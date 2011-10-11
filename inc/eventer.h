#ifndef EVENTER_H
#define EVENTER_H

#include "retriever.h"

#include <string>
#include <vector>

class Eventer {
     std::vector< Retriever > createQueue;
     std::vector< Retriever > readQueue;

     std::vector< std::string > tokenize( std::string data );
     std::vector< std::string > parseUrls( std::vector< std::string > tokens );

     public:

     Eventer();
     int start( int n );
     void retrieve( std::string url );
};

#endif 



