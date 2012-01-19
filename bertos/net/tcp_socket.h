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
 */

#ifndef NET_TCP_SOCKET_H
#define NET_TCP_SOCKET_H

#include <cfg/macros.h>

#include <io/kfile.h>

#include <lwip/netif.h>
#include <lwip/ip_addr.h>

#define ERR_TCP_NOTCONN    BV(0);
#define ERR_CONN_RESET     BV(1);
#define ERR_CONN_CLOSE     BV(2);
#define ERR_RECV_DATA      BV(3);


typedef struct TcpSocket
{
	KFile fd;
	struct netconn *sock;
	struct ip_addr *local_addr;
	struct ip_addr *remote_addr;
	uint16_t port;
	uint16_t error;
} TcpSocket;

#define KFT_TCPSOCKET MAKE_ID('T', 'S', 'C', 'K')

INLINE TcpSocket *TCPSOCKET_CAST(KFile *fd)
{
	ASSERT(fd->_type == KFT_TCPSOCKET);
	return (TcpSocket *)fd;
}


void tcpsocket_init(TcpSocket *socket, struct ip_addr *local_addr, struct ip_addr *remote_addr, uint16_t port);

#endif /* NET_TCP_SOCKET_H */
