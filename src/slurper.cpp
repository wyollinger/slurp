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

#include <iostream>

#include "eventer.h"
#include "scanner.h"
#include "uri.h"

using namespace slurp;

static int checkArgs( int argc, char** argv );

int main(int argc, char** argv) {
    int i, ret;
    const static int DEFAULT_URLS = 1024;
    Eventer* eventer = NULL;

    /* debugging for the flex code */
    SlurpScanner ss( 
        &std::stringstream( 
            std::string("< html><body><a href='tits'></a></body><body>"), 
             std::stringstream::in | std::stringstream::out ),
        &std::cout);

    ss.yylex();

    if( checkArgs( argc, argv ) ) {
        eventer = new Eventer();

        for( i = 1; i <= argc-1; i++ ) {
            eventer -> queueURI( URI( argv[i] ) );
        }

        eventer -> run( DEFAULT_URLS );
    } else {
        std::cerr << "insufficient or malformed arguments\n";
    }
    
    ret = eventer ? 0 : 1;
    delete eventer;
    return  ret; 
}

static int checkArgs( int argc, char** argv ) {
    int i, ret = 1;   
 
    if( argc < 2 ) {
      ret = 0;
    } else {


    }

    return ret;
}
