/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2012 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief TCP sockect with kfile interface.
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 * \author Daniele Basile <asterix@develer.com>
 *
 * notest:avr
 */

#include "tcp_socket.h"

#include "cfg/cfg_tcpsocket.h"

#define LOG_LEVEL   TCPSOCKET_LOG_LEVEL
#define LOG_FORMAT  TCPSOCKET_LOG_FORMAT
#include <cfg/log.h>
#include <cpu/byteorder.h>

#include <lwip/ip_addr.h>
#include <lwip/api.h>
#include <lwip/netif.h>
#include <lwip/netbuf.h>
#include <lwip/tcpip.h>


INLINE void close_socket(TcpSocket *socket)
{
	/* Clean all previuos states */
	netbuf_delete(socket->rx_buf_conn);
	socket->rx_buf_conn = NULL;
	socket->remaning_data_len = 0;

	if (!socket->sock)
		return;

	/* Close socket if was open */
	netconn_delete(socket->sock);
	socket->sock = NULL;
	return;
}

static bool tcpsocket_reconnect(TcpSocket *socket)
{
	LOG_ERR("Reconnecting..\n");

	/* Close socket if was open */
	close_socket(socket);

	/* If we are in server mode we do nothing */
	if (socket->handler)
		return false;

	/* Start with new connection */
	socket->sock = netconn_new(NETCONN_TCP);
	if(!socket->sock)
	{
		LOG_ERR("Unabe to alloc new connection\n");
		socket->error = -1;
		goto error;
	}

	socket->error = netconn_bind(socket->sock, socket->local_addr, socket->port);
	if(socket->error != ERR_OK)
	{
		LOG_ERR("Connection error\n");
		goto error;
	}

	socket->error = netconn_connect(socket->sock, socket->remote_addr, socket->port);
	if(socket->error != ERR_OK)
	{
		LOG_ERR("Cannot create socket\n");
		goto error;
	}

	LOG_INFO("connected ip=%d.%d.%d.%d\n", IP_ADDR_TO_INT_TUPLE(socket->remote_addr->addr));
	return true;

error:
	netconn_delete(socket->sock);
	socket->sock = NULL;
	return false;
}

static int tcpsocket_close(KFile *fd)
{
	TcpSocket *socket = TCPSOCKET_CAST(fd);
	close_socket(socket);
	socket->error = 0;
	return 0;
}

static KFile *tcpsocket_reopen(KFile *fd)
{
	TcpSocket *socket = TCPSOCKET_CAST(fd);
	if (tcpsocket_reconnect(socket))
		return fd;

	return NULL;
}

/*
 * Read data from socket.
 *
 * The read return the bytes that had been received if they are less than we request too.
 * Otherwise if the byte that we want read are less that the received bytes, we return only
 * the requested bytes. To get the remaning bytes we need to make an others read, until the
 * buffer is empty.
 * When there are not any more bytes, a new read takes data from remote socket.
 */
static size_t tcpsocket_read(KFile *fd, void *buf, size_t len)
{
	TcpSocket *socket = TCPSOCKET_CAST(fd);

	char *data;
	uint16_t read_len = 0;

	if (socket->remaning_data_len <= 0)
	{
		LOG_INFO("No byte left.\n");
		netbuf_delete(socket->rx_buf_conn);
	}
	else /* We had byte into buffer use that */
	{
		LOG_INFO("Read stored bytes.\n");
		if (!socket->rx_buf_conn)
		{
			LOG_ERR("Byte stored are corrupted!\n");
			socket->remaning_data_len = 0;
			return 0;
		}
		uint16_t tot_data_len = 0;
		netbuf_data(socket->rx_buf_conn, (void **)&data, &tot_data_len);

		if (data)
		{
			ASSERT(((int)tot_data_len - (int)socket->remaning_data_len) >= 0);
			size_t chunk_len = MIN((size_t)(socket->remaning_data_len), len);
			memcpy((char *)buf, &data[tot_data_len - socket->remaning_data_len], chunk_len);

			socket->remaning_data_len -= chunk_len;
			return chunk_len;
		}
		else
		{
			LOG_ERR("No valid data to read\n");
			socket->remaning_data_len = 0;
			netbuf_delete(socket->rx_buf_conn);
			return 0;
		}
	}

	/* Try reconnecting if our socket isn't valid */
	if ((socket->sock == NULL) && !tcpsocket_reconnect(socket))
		return 0;

	while (len)
	{
		LOG_INFO("Get bytes from socket.\n");
		socket->rx_buf_conn = netconn_recv(socket->sock);

		socket->error = netconn_err(socket->sock);
		if (socket->error != ERR_OK)
		{
			LOG_ERR("While recv %d\n", socket->error);
			close_socket(socket);
			return 0;
		}

		size_t chunk_len = 0;
		uint16_t data_len = 0;
		if (socket->rx_buf_conn)
		{
			netbuf_data(socket->rx_buf_conn, (void **)&data, &data_len);

			if (data)
			{
				chunk_len = MIN((size_t)data_len, len);
				memcpy(buf, data, chunk_len);

				socket->remaning_data_len = data_len - chunk_len;
			}

			if (socket->remaning_data_len <= 0)
			{
				netbuf_delete(socket->rx_buf_conn);
				socket->rx_buf_conn = NULL;
				socket->remaning_data_len = 0;
				return chunk_len;
			}
		}

		len -= chunk_len;
		read_len += chunk_len;
	}

	return read_len;
}

static size_t tcpsocket_write(KFile *fd, const void *buf, size_t len)
{
	TcpSocket *socket = TCPSOCKET_CAST(fd);

	/* Try reconnecting if our socket isn't valid */
	if ((socket->sock == NULL) && !tcpsocket_reconnect(socket))
		return 0;

	socket->error = netconn_write(socket->sock, buf, len, NETCONN_COPY);
	if (socket->error != ERR_OK)
	{
		LOG_ERR("While writing %d\n", socket->error);
		close_socket(socket);
		return 0;
	}

	return len;
}

static int tcpsocket_error(KFile *fd)
{
	TcpSocket *socket = TCPSOCKET_CAST(fd);
	return socket->error;
}

static void tcpsocket_clearerr(KFile *fd)
{
	TcpSocket *socket = TCPSOCKET_CAST(fd);
	socket->error = 0;
}


/**
 * Process all received connection from socket.
 *
 * \param fd opened tcp socket server kfile context.
 *
 */
void tcpsocket_serverPoll(KFile *fd)
{
	TcpSocket *socket = TCPSOCKET_CAST(fd);

	if (!socket->sock)
		socket->sock = netconn_accept(socket->server_sock);

	if (!socket->sock)
	{
		LOG_ERR("Unable to connect with client\n");
		return;
	}

	socket->handler(fd);
}

/**
 * Init tcp socket connection with kfile interface.
 *
 * \note the real connection will be performed only when we do the first kfile_read().
 * The read function is blocking and receive timeout is not settable, this depends to the
 * current version of lwip.
 *
 * \param socket tcp socket context.
 * \param local_addr device ip address.
 * \param remote_addr ip address that we would connect.
 * \param port tcp port that we would connect.
 */
void tcpsocket_init(TcpSocket *socket, struct ip_addr *local_addr, struct ip_addr *remote_addr, uint16_t port)
{
	memset(socket, 0, sizeof(TcpSocket));

	socket->local_addr = local_addr;
	socket->remote_addr = remote_addr;
	socket->port = port;

	socket->fd._type = KFT_TCPSOCKET;
	socket->fd.read = tcpsocket_read;
	socket->fd.error = tcpsocket_error;
	socket->fd.close = tcpsocket_close;
	socket->fd.write = tcpsocket_write;
	socket->fd.clearerr = tcpsocket_clearerr;
	socket->fd.reopen = tcpsocket_reopen;

}

/**
 * Init tcp server whit kfile interface.
 *
 * Start a tcp server on registered ethernet interface, and it calls the user handler
 * when receive data from accepted connection socket.
 *
 * \note Use the tcpsocket_serverPoll() function to process al connections.
 *
 * \param socket tcp socket context.
 * \param local_addr device ip address.
 * \param listen_addr ip address to listen
 * \param port tcp socket port to listen
 * \param handler user handler that server calls every time we recive a data from
 * socket. To the handler the server will pass the kfile context and user could choose
 * to make an explicit connection close, otherwise the server keep alive the connection.
 */
void tcpsocket_serverInit(TcpSocket *socket, struct ip_addr *local_addr, struct ip_addr *listen_addr, uint16_t port, tcphandler_t handler)
{
	tcpsocket_init(socket, local_addr, listen_addr, port);
	socket->handler = handler;

	socket->server_sock = netconn_new(NETCONN_TCP);
	socket->error = netconn_bind(socket->server_sock, listen_addr, port);
	socket->error = netconn_listen(socket->server_sock);

	if(socket->error != ERR_OK)
		LOG_ERR("Init server\n");
}
