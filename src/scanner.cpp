#include <cctype>

#include "scanner.h"

using namespace slurp;

Scanner::Scanner( const std::string& data ) {
    this->data = data;
    pos = 0;
}

std::string Scanner::next() {
    std::string next;
    YY_BUFFER_STATE buf;

    buf = yy_scan_string("test");

    yy_delete_buffer(buf);

    /* stub */
    /* todo: figure out how to call yylex/if it is needed */

    return next;
}



