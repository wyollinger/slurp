#include <iostream>
#include <sys/select.h>
#include "eventer.h"

Eventer::Eventer() {
    /* stub */
}

int Eventer::start( int n ) {
    int ret, urls, sock, max;
    std::vector< Retriever >::iterator ri;
    Retriever* cret = NULL;
    fd_set currentSockets;
    timeval socketTime;
   
    for( urls = 0; urls < n; ) {

       /* create new sockets, perform gets, and add descriptors to vector */
       while( !createQueue.empty() ) {
          cret =& createQueue.back();
          createQueue.pop_back();
          
          readQueue.push_back(*cret);
       }

       /* prepare for the call to select */
       FD_ZERO( &currentSockets );
       max = -1;
       for(ri = readQueue.begin(); ri != readQueue.end(); ri ++ ) {
          sock = ri -> createSocket();
          max = ( sock > max ) ? ( sock ) : ( max );
          FD_SET( sock, &currentSockets );
       }
       
       /* do the select and process the results*/
       socketTime.tv_sec = 0;
       socketTime.tv_usec = 100;
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
                   for( ri = readQueue.begin(); ri != readQueue.end(); ri ++ ) {                                 if( FD_ISSET( ri->getSocket(), &currentSockets ) ) {
                           /* do a read and call tokenize/parse+queue the results */
                           /* if the read fails discard the url as invalid noisily */
                           /* increment urls in here somewhere */
                       }
                   }
                   break;
           }
    }

    return urls;
}

void Eventer::retrieve( std::string url ) {
    createQueue.push_back( Retriever( url ) );
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

