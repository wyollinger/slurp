#include <iostream>
#include "eventer.h"

static int checkArgs( int argc, char** argv );

int main(int argc, char** argv) {
    int ret;
    Eventer* eventer = NULL;

    if( checkArgs( argc, argv ) ) {
        eventer = new Eventer();
        eventer -> retrieve( argv[1] );
        eventer -> start();
    } else {
        std::cerr << "checkArgs failed\n";
    }
    
    ret = eventer ? 0 : 1;
    delete eventer;
    return  ret; 
}

static int checkArgs( int argc, char** argv ) {
    /* stub */
    return 1;
}
