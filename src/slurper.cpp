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

#include <QQueue>
#include <QString>

#include <cstdlib>
#include <cstring>

#include <curl/curl.h>

#include "eventer.h"
#include "scanner.h"

const static char* USAGE_MESSAGE = "slurp [options] urls";

using namespace slurp;

static int validateArgs( int argc, char** argv, char** env, 
    QQueue<QString>& seedURIs, int& quota, int& maxThreads);
static void die( const char* errmsg, int errcode );

int main(int argc, char** argv, char** env) {
  int flags;
  int quota = -1;
  int maxThreads = 32;

  QQueue<QString> seedURIs;
  
  flags = validateArgs( argc, argv, env, seedURIs, quota, maxThreads );

  if( !flags ) {
     die(USAGE_MESSAGE, 1);
  }

  Eventer ev(seedURIs, quota, maxThreads);

  return ev.run(); 
}

static int validateArgs( int argc, char** argv, char** env, 
    QQueue<QString>& seedURIs, int& quota, int& maxThreads ) {
    int i;
    int flags = 0;

    if( argc == 1 ) { 
      /* no urls were passed, so return failure */
      return flags;
    }

    for( i = 1; i < argc; i++ ) {
       if( argv[i][0] == '-' ) {
         std::cout << "stub: process option: " 
		   << argv[i] 
		   << std::endl;
       } else {
         seedURIs.enqueue( argv[i] );
       }
    }

    for( i = 0; env[i]; i++ ) {
       /* search for relevant environment variables */
    }

    /* todo: use flags to pass option effects back to main */
    flags = 1;
    return flags;
}

static void die( const char* errmsg, int errcode )
{
    std::cerr << errmsg << std::endl;
    exit( errcode );
}
