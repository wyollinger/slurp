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

#include <event2/event-config.h>
#include <event2/util.h>

#include "eventer.h"
#include "scanner.h"

const static char* USAGE_MESSAGE = "slurp [options] urls";
const static int VERSION_ID[3] = {0,0,3};

using namespace slurp;

static void initLibraries();
static int validateArgs( int argc, char** argv, char** env, 
    QQueue<QString>& seedURIs, int& quota, int& maxThreads);
static void die( const char* errmsg, int errcode );

int main(int argc, char** argv, char** env) {
  int flags;
  int quota = -1; /* no quota */
  int maxThreads = 512;
  QQueue<QString> seedURIs;

  initLibraries();

  flags = validateArgs( argc, argv, env, seedURIs, quota, maxThreads );

  if( !flags ) {
     die(USAGE_MESSAGE, 1);
  }

  Eventer ev(seedURIs, quota, maxThreads);

  return ev.run(); 
}

static void initLibraries() {
   /* insert some macro magic here for a windows compile */	
   if( evthread_use_pthreads() ) {
      die("could not initialize libevent with pthreads", 2 );
   }

   if( curl_global_init( CURL_GLOBAL_ALL ) ) {
      die("could not initialize libcurl", 3 );
   }
}

static int validateArgs( int argc, char** argv, char** env, 
    QQueue<QString>& seedURIs, int& quota, int& maxThreads ) {
    int i;
    int flags = 0;

    if( argc == 1 ) { 
      /* no urls were passed, so return failure */
      return flags;
    }

    flags = 1;

    for( i = 1; i < argc; i++ ) {
       if( argv[i][0] == '-' ) {
	 switch( argv[i][1] ) {
            case 'V':
              flags = 0;      
              
	      std::cout << "slurp v" 
		        << VERSION_ID[0] << "."
		        << VERSION_ID[1] << "."
		        << VERSION_ID[2] << std::endl;

              std::cout << "using " 
		        << (LIBEVENT_VERSION) << std::endl;
              std::cout << "using "
		        << curl_version( ) << std::endl;

	      std::cout << "refer to COPYING file for license information\n";
              die("done.",EXIT_SUCCESS);
            break;

	    default:
              std::cout << "stub: process option: " 
		   << argv[i] 
		   << std::endl;
	    break;
	 }
      } else {
         seedURIs.enqueue( argv[i] );
       }
    }

    for( i = 0; env[i]; i++ ) {
       /* search for relevant environment variables */
    }

    /* todo: use flags to pass option effects back to main */
    return flags;
}

static void die( const char* errmsg, int errcode )
{
    std::cerr << errmsg << std::endl;
    exit( errcode );
}
