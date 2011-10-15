#ifndef URI_H
#define URI_H

#include <string>

namespace slurp {
  class URI {
    std::string rawUri;

    public:

    URI( );
    URI( std::string rawUri );
    static bool isValid( std::string rawUri ) {
      bool valid;
 
      valid = false;

      return valid;
    }
    bool isValid();
  };
}

/* 
       stub - we will need another flex file for this
        
       URI = scheme ":" hier-part [ "?" query ] [ "#" fragment ]

       hier-part   = "//" authority path-abempty
            / path-absolute
            / path-rootless
            / path-empty
*/


#endif
