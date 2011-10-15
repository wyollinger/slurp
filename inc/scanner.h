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

#ifndef SCANNER_H
#define SCANNER_H

#include <errno.h>
#include <string>
#include <vector>

#include "scanner.h"
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
     std::vector<std::string>* tokens;
     std::vector<std::string>::const_iterator ti;
     int pos;

     public:

     Scanner( const std::string& data );
     ~Scanner();
     std::string next();
     void scan();
  };

   std::vector<std::string>* scanHTML( const char* html );
}

#endif 
