#ifndef EVENTER_H
#define EVENTER_H

#include <string>
#include <vector>

class Eventer {

     std::vector< std::string > tokenize( std::string data );
     std::vector< std::string > parseUrls( std::vector< std::string > tokens );

     public:

     Eventer();
     int start( int n = -1);
     void retrieve( std::string url );
};

#endif 



