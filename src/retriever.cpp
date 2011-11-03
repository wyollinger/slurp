/*
 * Slurp - a web crawler
 * Copyright (C) 2011 Joseph Max DeLiso

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define _POSIX_SOURCE

#include <iostream>
#include <cstring>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "retriever.h"
#include "uri.h"

using namespace slurp;

Retriever::Retriever( const URI& uri ) {
    this-> uri = uri;
}

void Retriever::initiateRequest() {
    std::string hostname = uri.getRawData();
    addrinfo hints, *result, *rp;   
    int ret;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* TCP socket */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_flags = AI_ADDRCONFIG | AI_V4MAPPED | AI_ALL;

    if( !( ret = getaddrinfo(hostname.c_str(), "80", &hints, &result)) ) 
    {
       std::cerr << "getaddrinfo: " << gai_strerror(ret) << "\n";
       return;    
    }

    for( rp = result; rp; rp = rp->ai_next) {
        if( ( sock = socket( rp->ai_family, rp->ai_socktype, rp->ai_protocol) ) 
	   != -1 ) {

           rp = NULL; /* signal a break */
	} 
    }

    freeaddrinfo(result);

    if( sock != -1 ) {
       	/* we managed to connect, so now do the HTTP GET */
    }
}

std::string Retriever::readData() {
    std::string data;

    /* stub */

    return data;
}

