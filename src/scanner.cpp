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

#include <cctype>
#include <iostream>

#include "scanner.h"
#include "retriever.h"

using namespace slurp;

Scanner::Scanner( const std::string& data ) {
    tokens = NULL;
    this->data = data;
    pos = 0;
}

Scanner::~Scanner() {
   if( tokens ) {
      delete tokens;
   }
}

std::string Scanner::next() {
    std::string next;

    /* simply iterate accross tokens */

    return next;
}

void Scanner::scan() {
   if( tokens ) {
      delete tokens;
   }

   /* do a retrieval here */

   /* call scanHTML on the results */
}
