#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>

namespace slurp {
 class URI {
    std::string uri;

    public:

    URI( std::string uri );
    static bool isValid( std::string uri ) {
      bool valid;
 
      valid = true;
      /* stub */

      return valid;
    }
  };

  class Parser {
    std::vector< std::string > tokens;

    public:

    Parser( const std::vector< std::string >& tokens );
    std::vector< URI > parseLinks() const;
  }; 
}

#endif 

