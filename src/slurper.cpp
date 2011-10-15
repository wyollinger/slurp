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

using namespace slurp;

static int checkArgs( int argc, char** argv );
static void doUnitTests();

int main(int argc, char** argv) {
    int i, ret;
    const static int DEFAULT_URLS = 1024;
    Eventer* eventer = NULL;

    doUnitTests(); 

    if( checkArgs( argc, argv ) ) {
        eventer = new Eventer();

        for( i = 1; i <= argc-1; i++ ) {
            eventer -> retrieve( argv[i] );
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
    int i, ret;

    if( argc < 2 ) {
       ret = 0;
    } else {
       ret = 1;

       /* validate the urls */
       for( i = 1; ret && i <= argc-1; i++ ) {
          if( !URI::isValid( argv[i] ) ) {
            ret = 0;
          }
       }
    }

    return ret;
}

static void doUnitTests() {
    const char testBlock[] = "<html><head><title>test</title></head><body><a href='index.html'</body></html>";
    std::vector<std::string>* tagVector = new std::vector<std::string>();
    yyscan_t scanner;

    yylex_init(&scanner);
    yylex_init_extra(tagVector, &scanner);

    yy_scan_string(testBlock, scanner);
    yylex(scanner);

    std::cout << "got " << tagVector->size() << " elements\n";
    std::cout << "test complete\n";

    yylex(scanner);
    yylex_destroy(scanner);
    delete tagVector; 
}
