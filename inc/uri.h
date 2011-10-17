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

#ifndef URI_H
#define URI_H

#include <string>
#include <vector>

namespace slurp {
  class URI {
    std::string rawUri;
    std::string proto;
    std::vector< std::string> hnameParts;
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
