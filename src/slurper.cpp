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
#include <QtGlobal>
#include <QThread>

#include <cstdlib>
#include <cstring>

#include <curl/curl.h>

#include <event2/event-config.h>
#include <event2/util.h>

#include "globals.h"
#include "retriever.h"
#include "eventer.h"
#include "scanner.h"

using namespace slurp;

static void initLibraries();
static int validateArgs( int argc, char** argv, char** env, 
    QQueue<QString>& seedURIs, int& quota, int& maxThreads);

int main(int argc, char** argv, char** env) {
  int flags;
  int quota = -1; /* no quota */
  int maxThreads = QThread::idealThreadCount();
  QQueue<QString> seedURIs;

  initLibraries();

  flags = validateArgs( argc, argv, env, seedURIs, quota, maxThreads );

  if( !flags ) {
     die(USAGE_MESSAGE, 1);
  } else if( seedURIs.size() == 0 ) {
     die(
        "error: you must specify the initial URI to begin crawling from", 
	EXIT_FAILURE );
  }

  if( flags & FLAGS_VERBOSE ) {
    std::cout << "information: creating eventer instance with "
	      << seedURIs.size() << " seed URI(s) and a quota of " 
	      << quota << " with the contraint of " 
	      << maxThreads << " maximum threads\n";
  }

  Eventer ev(seedURIs, quota, maxThreads, flags);

  return ev.run(); 
}

static void initLibraries() {
   /* insert some macro magic here for a windows compile */	
   if( evthread_use_pthreads() ) {
      die("error: could not initialize libevent with pthreads", EXIT_FAILURE );
   }

   if( curl_global_init( CURL_GLOBAL_ALL ) ) {
      die("error: could not initialize libcurl", EXIT_FAILURE );
   }
}

static int validateArgs( int argc, char** argv, char** env, 
    QQueue<QString>& seedURIs, int& quota, int& maxThreads ) {
    int i;
    int flags = FLAGS_INVALID;

    if( argc == 1 ) { 
      /* no urls were passed, so return failure */
      return flags;
    }

    flags = FLAGS_VALID;

    for( i = 1; i < argc; i++ ) {
       if( argv[i][0] == '-' ) {
	 switch( argv[i][1] ) {
            case 'V':
              flags = 0;      
              
	      std::cout << "slurp v" 
		        << VERSION_ID[0] << "."
		        << VERSION_ID[1] << "."
		        << VERSION_ID[2] 
                        << "\nusing libevent " 
		        << event_get_version() 
                        << "\nusing " << curl_version()
			<< "\nusing QT " << qVersion() << "\n";

	      die(LICENSE_INFO, 
                  EXIT_SUCCESS);
            break;

	    case 'h':
              die(HELP_MENU, EXIT_SUCCESS);
	      break;

	    case 'v':
              flags |= FLAGS_VERBOSE;
	      break;

	    case 'n':
	      if( strlen(argv[i]+2) ) {
                quota = atoi((argv[i]+2));
	      } else if( i+1 < argc ) {
                quota = atoi (argv[i+1]);
		i++;
	      } else {
	        die(
	           "error: could not find numeric portion of -n option",
		   EXIT_FAILURE);
	      }

	      break;

	    case 't':
              if( strlen(argv[i]+2) ) {
                maxThreads = atoi((argv[i]+2));
	      } else if( i+1 < argc ) {
                maxThreads = atoi ( argv[i+1] );
		i++;
	      } else {
	        die(
                   "error: could not find numeric portion of -t option",
		   EXIT_FAILURE);
	      }

	      if( maxThreads <= 0 ) {
                die("error: t must be greater than zero", EXIT_FAILURE);
	      }

	      break;

	    default:
              std::cout << "warning: unrecognized option: " 
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

    return flags;
}

void die( const char* errmsg, int errcode )
{
    std::cerr << errmsg << std::endl;
    exit( errcode );
}
