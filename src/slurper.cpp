#include <iostream>
#include "eventer.h"

using namespace slurp;

static int checkArgs( int argc, char** argv );

int main(int argc, char** argv) {
    int i, ret;
    const static int DEFAULT_URLS = 1024;
    Eventer* eventer = NULL;

    if( checkArgs( argc, argv ) ) {
        eventer = new Eventer();

        for( i = 1; i <= argc-1; i++ ) {
            eventer -> retrieve( argv[i] );
        }

        eventer -> run( DEFAULT_URLS );
    } else {
        std::cerr << "checkArgs failed\n";
    }
    
    ret = eventer ? 0 : 1;
    delete eventer;
    return  ret; 
}

static int checkArgs( int argc, char** argv ) {
    int ret;

    if( argc < 2 ) {
       ret = 0;
    } else {
       /* validate the urls */
       ret = 1;
    }

    return ret;
}
