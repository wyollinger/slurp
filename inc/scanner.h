#ifndef SCANNER_H
#define SCANNER_H

#include <errno.h>
#include <string>

#include "htmlscanner.h"

namespace slurp {
 
 
   class URI {
    std::string uri;

    public:

    URI( std::string uri );
    static bool isValid( std::string uri ) {
      bool valid;
 
      valid = false;
      /* stub */

      return valid;
    }
  };

  class Scanner {
     std::string data;
     int pos;

     public:

     Scanner( const std::string& data );
     std::string next();
     int yylex();
  };
}

#endif 
