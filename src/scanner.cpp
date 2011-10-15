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

using namespace slurp;

Scanner::Scanner( const std::string& data ) {
    this->data = data;
    pos = 0;
}

std::string Scanner::next() {
    std::string next;
    /*
    something like this..

    YY_BUFFER_STATE buf;
    buf = yy_scan_string("test");
    yy_delete_buffer(buf);
     */

    /* stub */
    /* todo: figure out how to call yylex/if it is needed */

    return next;
}
