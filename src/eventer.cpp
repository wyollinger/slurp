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
#include <sys/select.h>

#include "eventer.h"
#include "uri.h"

using namespace slurp;

Eventer::Eventer() {
    /* stub */
}

int Eventer::run( int n ) {
    int ret, urls, sock, max;
    std::vector< Retriever >::iterator ri;
    fd_set currentSockets;
    timeval socketTime;
   
    std::cout << "Eventer::run()\n";

    for( urls = 0; urls < n; ) {
       socketTime.tv_sec = 0;
       socketTime.tv_usec = 100;
       FD_ZERO( &currentSockets );
       max = -1;

       /* empty create queue into read queue, creating sockets for each url */
       /* there will be a mutex needed here */
       while( !createQueue.empty() ) {
          Retriever& cret = createQueue.back(); /* get reference to next retriever */
          cret.initiateRequest();                  /* perform the request to get the socket */
          readQueue.push_back(cret);            /* put this retriever data into the readQueue */
          createQueue.pop_back();               /* remove the retriever object from the create queue */
       }

       /* prepare for the call to select, all sockets have been created already */
       for(ri = readQueue.begin(); ri != readQueue.end(); ri ++ ) {
          sock = ri -> getSocket(); 
          max = ( sock > max ) ? ( sock ) : ( max );
          FD_SET( sock, &currentSockets );
       }
       
       /* do the select and process the results*/
       switch( 
           ret = select( max+1, 
                   &currentSockets, 
                   NULL, 
                   &currentSockets, 
                   &socketTime ) )
           {
               case -1:
                   std::cerr << "select()\n"; 
                   return urls;
               case 0:
                   break;
               default:
                   for( ri = readQueue.begin(); ri != readQueue.end(); ri ++ ) {                                 
                      if( FD_ISSET( ri->getSocket(), &currentSockets ) ) {
                           /* do a read and call tokenize/parse+queue the results */
                           /* if the read fails discard the url as invalid noisily */
                           /* increment urls in here to avoid infinite loop */
                      }
                   }
                   break;
           }
    }

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
