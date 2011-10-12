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
    yyscan_t scanner;

    yylex_init(&scanner);
    yy_scan_string(testBlock, scanner);
    yylex(scanner);
    yylex_destroy(scanner);
}
