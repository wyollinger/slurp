#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>

class Parser {
     std::vector< std::string > tokens;

     public:

     Parser( const std::vector< std::string >& tokens );
     std::vector< std::string > parseLinks() const;
};

#endif 

