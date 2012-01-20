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

#define LOG_LEVEL   LOG_LVL_INFO
#define LOG_FORMAT  LOG_FMT_TERSE
#include <cfg/log.h>
#include <cpu/byteorder.h>

#include <lwip/ip_addr.h>
#include <lwip/api.h>
#include <lwip/netif.h>
#include <lwip/netbuf.h>
#include <lwip/tcpip.h>

static int tcpConnect(TcpSocket *socket)
{
	socket->remaning_data_len = 0;
	socket->sock = netconn_new(NETCONN_TCP);
	ASSERT(socket->sock);

	if(netconn_bind(socket->sock, socket->local_addr, socket->port) != ERR_OK)
	{
		LOG_ERR("Connection error\n");
		goto error;
	}

	if(netconn_connect(socket->sock, socket->remote_addr, socket->port) != ERR_OK)
	{
		LOG_ERR("Cannot create socket\n");
		goto error;
	}

	LOG_INFO("connected ip=%d.%d.%d.%d\n", IP_ADDR_TO_INT_TUPLE(socket->local_addr->addr));
	return 0;

error:
	netconn_delete(socket->sock);
	socket->sock = NULL;
	return -1;
}


static bool reconnect(TcpSocket *socket)
{
	LOG_INFO("Reconnecting...\n");
	// Release old socket if needed
	if (socket->sock)
	{
		if (netconn_delete(socket->sock) != ERR_OK)
			LOG_ERR("Error closing socket\n");

		socket->sock = NULL;
	}

	// Connect to our peer peer
	if (tcpConnect(socket) < 0)
	{
		LOG_ERR("Reconnect error!\n");
		socket->error |= ERR_TCP_NOTCONN;
		return false;
	}

	LOG_INFO("Reconnecting DONE!\n");

	return true;
}

static int tcpsocket_close(KFile *fd)
{
	TcpSocket *socket = TCPSOCKET_CAST(fd);
	int ret = netconn_delete(socket->sock);
	socket->sock = NULL;

	if (ret)
	{
		LOG_ERR("Close error\n");
		socket->error |= ERR_CONN_CLOSE;
		return EOF;
	}
	return 0;
}

static size_t tcpsocket_read(KFile *fd, void *buf, size_t len)
{
	TcpSocket *socket = TCPSOCKET_CAST(fd);
	char *_buf;
	uint16_t read_len = 0;
	uint16_t recv_data_len = 0;
	size_t _len = 0;

	if (socket->remaning_data_len == 0)
	{
		LOG_INFO("No byte left.\n");
		if (socket->rx_buf_conn)
			netbuf_delete(socket->rx_buf_conn);
	}
	else if (socket->remaning_data_len > 0)
	{
		LOG_INFO("Return stored bytes.\n");
		ASSERT(socket->rx_buf_conn);
		netbuf_data(socket->rx_buf_conn, (void **)&_buf, &recv_data_len);

		if (_buf)
		{
			ASSERT((recv_data_len - socket->remaning_data_len) > 0);
			_len = MIN((size_t)(socket->remaning_data_len), len);
			memcpy((char *)buf, &_buf[recv_data_len - socket->remaning_data_len], _len);

			socket->remaning_data_len -= _len;
			return _len;
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
	if (!socket->sock)
	{
		if (!reconnect(socket))
			return 0;
	}

	while (len)
	{
		LOG_INFO("Get bytes from socket.\n");
		socket->rx_buf_conn = netconn_recv(socket->sock);
		socket->error = netconn_err(socket->sock);

		if (socket->error != ERR_OK)
		{
			LOG_ERR("While recv %d\n", socket->error);
			socket->rx_buf_conn = NULL;
			return 0;
		}

		if (socket->rx_buf_conn)
		{
			netbuf_data(socket->rx_buf_conn, (void **)&_buf, &recv_data_len);
			if (_buf)
			{
				socket->remaning_data_len = recv_data_len;
				_len = MIN((size_t)recv_data_len, len);
				memcpy(buf, _buf, _len);
				socket->remaning_data_len -= _len;
			}

			if (socket->remaning_data_len <= 0)
			{
				netbuf_delete(socket->rx_buf_conn);
				return _len;
			}
		}

		len -= _len;
		read_len += _len;
	}

	return read_len;
}

static size_t tcpsocket_write(KFile *fd, const void *buf, size_t len)
{
	TcpSocket *socket = TCPSOCKET_CAST(fd);

	// Try reconnecting if our socket isn't valid
	if (!socket->sock)
	{
		if (!reconnect(socket))
			return 0;
	}

	int result = netconn_write(socket->sock, buf, len, NETCONN_COPY);
	if (result != ERR_OK)
	{
		LOG_ERR("While writing %d\n", result);
		if (result == ERR_RST)
		{
			LOG_INFO("Connection close\n");

			if (tcpsocket_close(fd) == EOF)
				LOG_ERR("Error closing socket, leak detected\n");
			return 0;
		}
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

void tcpsocket_init(TcpSocket *socket, struct ip_addr *local_addr, struct ip_addr *remote_addr, uint16_t port)
{
	socket->sock = NULL;
	socket->local_addr = local_addr;
	socket->remote_addr = remote_addr;
	socket->port = port;

	socket->fd._type = KFT_TCPSOCKET;
	socket->fd.read = tcpsocket_read;
	socket->fd.error = tcpsocket_error;
	socket->fd.close = tcpsocket_close;
	socket->fd.write = tcpsocket_write;
	socket->fd.clearerr = tcpsocket_clearerr;

}
