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
#include <cstdlib>

#include "eventer.h"
#include "retriever.h"
#include "scanner.h"

const static char* USAGE_MESSAGE = "slurp [options] urls";

using namespace slurp;

static int validateArgs( int argc, char** argv, char** env);
static void die( const char* errmsg, int errcode );

int main(int argc, char** argv, char** env) {
     if( !validateArgs( argc, argv, env ) ) {
        die(USAGE_MESSAGE, 1);
     }

     return  EXIT_SUCCESS; 
}

static int validateArgs( int argc, char** argv, char** env ) {

    return 0;
}

static void die( const char* errmsg, int errcode )
{
    std::cerr << errmsg << std::endl;
    exit( errcode );
}
