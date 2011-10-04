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
 * Copyright 2011 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Simple Http server error pages.
 *
 * \author Daniele Basile <asteix@develer.com>
 */

#include "hw_http.h"

const char http_file_not_found[] = "\
<!DOCTYPE html PUBLIC \"-//IETF//DTD HTML 2.0//EN\"> \
<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\"> \
<title>404 Not Found</title></head><body><img src=\"bertos_logo_jpg\"><h1>404 Not Found</h1>\
<p>The requested URL was not found on this server.</p><hr>\
<address>BeRTOS simple HTTP server</address></body></html>";

const size_t http_file_not_found_len = sizeof(http_file_not_found);

const char http_sd_not_present[] = " \
<!DOCTYPE html PUBLIC \"-//IETF//DTD HTML 2.0//EN\">  \
<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\">  \
<title>BeRTOS simple HTTP Server</title></head><body><img src=\"bertos_logo_jpg\"> \
<h1>BeRTOS simple HTTP Server</h1><p>Simple Http server, the site's pages are stored on SD card, check it if is present.</p><hr>\
<a href=\"http://www.bertos.org\">www.BeRTOS.org</a></body></html> \
";
const size_t http_sd_not_present_len = sizeof(http_sd_not_present);

const char http_server_error[] = " \
<!DOCTYPE html PUBLIC \"-//IETF//DTD HTML 2.0//EN\"> \
<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\"> \
<title>500 Internal Server error</title></head><body><img src=\"bertos_logo_jpg\"><h1>500 Internal Server error</h1>\
<p>The internal server error was occur while processing the requested page.</p><hr>\
<address>BeRTOS simple HTTP server</address></body></html>";

const size_t http_server_error_len = sizeof(http_server_error);
