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
#include <QDebug>
#include <curl/curl.h>

#include "callbacks.h"
#include "eventer.h"
#include "util.h"

namespace slurp {

void eventCallback(int fd, short kind, void *userp)
{
  Eventer* eventer = reinterpret_cast< Eventer* > ( userp );
 
  qDebug() << "debug: in event callback with fd " 
	   << fd << " kind " 
	   << kind;

  eventer -> processSocketEvent(fd, kind);
  eventer -> scanMultiInfo();
  eventer -> checkTimer();
}
 
void timerCallback(int fd, short kind, void* userp) 
{
  Eventer* eventer = reinterpret_cast< Eventer* > ( userp );

  (void)fd;
  (void)kind;

  eventer -> updateTimer();
  eventer -> scanMultiInfo();
}

int multiTimerCallback(
        CURLM *multi_handle, 
	long timeout_ms,
	void *userp)
{
       Eventer* eventer = reinterpret_cast< Eventer* > ( userp );

    (void)multi_handle;

    eventer -> addTimer( timeout_ms );

    return 0;
}

int socketCallback(
        CURL *e, 
	curl_socket_t s, 
	int what, 
	void *userp_a, 
	void *userp_b)
{
  const static char *whatLut[] = { "none", "IN", "OUT", "INOUT", "REMOVE" };
  Eventer* eventer = reinterpret_cast< Eventer* > ( userp_a );
  Retriever* retriever = reinterpret_cast< Retriever* > ( userp_b );

  qDebug() << "debug: socket callback: socket " << s
	    << "easy handle: " << e 
	    << "event: " << whatLut[what] 
	    << "eventer @" << eventer
            << "retriever @" << retriever;

  if (what == CURL_POLL_REMOVE) {
      qDebug() << "debug: remove stub";
  } else {
    if (!retriever) {
      eventer -> addSocket(s, e, what); 
    } else {
      eventer -> setSocket( retriever, s, e, what ); 
    }
  }

  return 0;
}
 
size_t writeCallback(
        void *ptr, 
	size_t size, 
	size_t nmemb, 
	void *data)
{
  size_t realsize = size * nmemb;
 
  (void)ptr;
  (void)data;

  return realsize;
}
 
int progressCallback(
	void *p, 
	double dltotal, 
	double dlnow, 
	double ult,
        double uln)
{
  (void)p;
  (void)dltotal;
  (void)dlnow;
  (void)ult;
  (void)uln;

 return 0;
}

void keyboardCallback(
        evutil_socket_t s,
	short type, 
	void *userp)
{
  Eventer* eventer = reinterpret_cast< Eventer* > ( userp );
  QString userInput = "";
  char cchar;

  qDebug() << "debug: in keyboard callback with socket s "
	    << s << " type "
	    << type;

  do {
    cchar = std::cin.get();

    if( cchar != '\n' ) {
        userInput += cchar;
    } 
  } while( cchar != '\n' );

  qDebug() << "debug: in kbcallback with <" 
           << userInput.toAscii().data() 
           << ">";

  /* 
     TODO: parse and process commands? 
     
     ..otherwise, treat string as URI and attempt to retrieve it
  */

  if( userInput.size() > 0 ) {
     eventer -> queueURI( userInput );
  }
}

} /* namespace slurp */
