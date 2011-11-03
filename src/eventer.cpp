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

#include <iostream>
#include <sys/epoll.h>
#include <cstdio>
#include <cerrno>

#include "eventer.h"
#include "uri.h"

using namespace slurp;

Eventer::Eventer() {
    /* stub */
}

int Eventer::run( int n ) {
    int ret, urls, sock, epollfd, i;
    std::vector< Retriever >::iterator ri;
    fd_set currentSockets;
    epoll_event ev, *epollEvents;
    const int MAX_EVENTS = 256;

    epollEvents = new epoll_event[MAX_EVENTS];

    std::cout << "Eventer::run()\n";

    if( ( epollfd = epoll_create( MAX_EVENTS )) == -1 ) { /* change so not using magic number */
        return -1;
    }

    for( urls = 0; urls < n; ) {
       /* empty create queue into read queue, creating sockets for each url */
       /* there will be a mutex needed here */
       while( !createQueue.empty() ) {
          Retriever& cret = createQueue.back(); /* get reference to next retriever */
          cret.initiateRequest();                  /* perform the request to get the socket */
          readQueue.push_back(cret);            /* put this retriever data into the readQueue */
          createQueue.pop_back();               /* remove the retriever object from the create queue */
       }

       /* prepare for the call to epoll, all sockets have been created already */
       for(ri = readQueue.begin(); ri != readQueue.end(); ri ++ ) {
          sock = ri -> getSocket(); 
	  ev.events = EPOLLIN;
	  ev.data.fd = sock;
	  if( epoll_ctl( epollfd, EPOLL_CTL_ADD, sock, &ev ) == -1 ) {
             perror("epoll_ctl: adding sock from read queue");
	  }
       }
       
       /* 
	* a call to epollctl( epollfd, EPOLL_CTL_DEL, sock, NULL ) 
	* is needed is needed in the parse/queue thread
	*
	*/

       /* do the epoll and process the results*/
       switch( ret = epoll_wait( epollfd, epollEvents, MAX_EVENTS, -1 ) )
           {
               case -1:
                   perror("epoll_wait");
                   return urls;
               case 0:
                   break;
               default:
                   for( i = 0; i < ret; i ++ ) {                                 
	              /* spawn a thread for events[i] */
                   }
                   break;
           }
    }

    delete epollEvents;
    return urls;
}

void Eventer::queueURI( URI uri ) {
  if( uri.isValid() ) {
    createQueue.push_back( Retriever( uri ) );
  }
}

std::vector< std::string > Eventer::tokenize( std::string data ) {
    std::vector< std::string > tokens;

    /* stub */

    return tokens;
}

std::vector< std::string > Eventer::parseUrls( std::vector< std::string > tokens ) {
    std::vector< std::string > urls;

    /* stub */
    
    return urls;
}
