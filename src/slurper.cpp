#include <iostream>
#include "slurper.h"

static int checkArgs( int argc, char** argv );

int main(int argc, char** argv) {
    int ret;

    if( checkArgs( argc, argv ) ) {
        /* create eventer and send it an initial URL, begin event loop */
        ret = 0;
    } else {
        std::cerr << "error stub\n";
        ret = 1;
    }

    return ret;
}

static int checkArgs( int argc, char** argv ) {
    /* stub */
    return 1;
}
