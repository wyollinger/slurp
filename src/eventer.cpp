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

#include "globals.h"
#include "retriever.h"
#include "scanner.h"

using namespace slurp;

Eventer::Eventer( 
    const QQueue<QString>& initialURIs, 
    int quota, 
    int maxThreads,
    int flags ) {
      pendingURIs = initialURIs;
      this -> quota = quota;
      this -> flags = flags;

      threadPool.setMaxThreadCount( maxThreads );

      multi = curl_multi_init();

      curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, socketCallback);
      curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, timerCallback);

      eventPtr = event_base_new();
}

Eventer::~Eventer() {
  event_base_free( eventPtr );
  curl_multi_cleanup(multi);
}

void Eventer::curlVerify(const char *where, CURLMcode code)
{
  if ( CURLM_OK != code ) {
    const char *s;
    switch (code) {
      case     CURLM_CALL_MULTI_PERFORM: s="CURLM_CALL_MULTI_PERFORM"; break;
      case     CURLM_BAD_HANDLE:         s="CURLM_BAD_HANDLE";         break;
      case     CURLM_BAD_EASY_HANDLE:    s="CURLM_BAD_EASY_HANDLE";    break;
      case     CURLM_OUT_OF_MEMORY:      s="CURLM_OUT_OF_MEMORY";      break;
      case     CURLM_INTERNAL_ERROR:     s="CURLM_INTERNAL_ERROR";     break;
      case     CURLM_UNKNOWN_OPTION:     s="CURLM_UNKNOWN_OPTION";     break;
      case     CURLM_LAST:               s="CURLM_LAST";               break;
      default: s="CURLM_unknown";
        break;
    case     CURLM_BAD_SOCKET:         s="CURLM_BAD_SOCKET";
       std::cerr << "ERROR: " << where << "returns" << s << std::endl;
      return;
    }
    std::cerr << "ERROR: " << where << "returns" << s << std::endl;
  }
}

int Eventer::run() {
  int ret;

  //threadPool.start( new QRunnable() );
  //call libevent to start processing events
  
  ret = event_base_loop( eventPtr, 0 );

  std::cout << "debug: event loop returned " << ret << "\n";

  return ret;
}

void Eventer::queueURI( const QString& uri ) {
  pendingURIs.enqueue( uri );
}

void Eventer::eventCallback(int fd, short kind, void *userp)
{
 std::cout << "debug: in event callback with fd " 
	   << fd << " kind " 
	   << kind << "\n";
}
 
void Eventer::timerCallback(int fd, short kind, void *userp) 
{
 std::cout << "debug: in timer callback with fd " 
	   << fd << " kind " 
	   << kind << "\n";
}

int Eventer::socketCallback(
        CURL *e, 
	curl_socket_t s, 
	int what, 
	void *cbp, 
	void *sockp)
{
 std::cout << "debug: in socket callback with what " 
	   << what << "\n";
}
 
size_t Eventer::writeCallback(
        void *ptr, 
	size_t size, 
	size_t nmemb, 
	void *data)
{
 std::cout << "debug: in write callback with fd " 
	   << size << " bytes to write\n"; 
}
 
int Eventer::progressCallback(
	void *p, 
	double dltotal, 
	double dlnow, 
	double ult,
        double uln)
{
 std::cout << "debug: in progress callback with fd " 
	   << (dlnow/dltotal*100.0) << "% complete\n"; 
}

