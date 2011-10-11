#ifndef EVENTER_H
#define EVENTER_H

#include <string>
#include <vector>

class Eventer {

     public:
     Eventer();

     int start( int n = -1);
     void retrieve( std::string url );
     void tokenize( std::string data );
     void parse( std::vector< std::string > tokens );
};

#endif 



