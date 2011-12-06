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
#include <curl/curl.h>

#include "callbacks.h"

namespace slurp {

void eventCallback(int fd, short kind, void *userp)
{
  Eventer *eventer = (Eventer*) userp;
  CURLMcode rc;
  CURLM* multiHandle;
  int action, running;

  action =
    ( kind & EV_READ ? CURL_CSELECT_IN : 0 ) |
    ( kind & EV_WRITE ? CURL_CSELECT_OUT : 0 );

  qDebug() << "debug: in event callback with fd " 
	   << fd << " kind " 
	   << kind << " action "
	   << action;

  /* FIXME: move to eventer::processEvent */

  multiHandle = eventer -> getMultiHandle();

  rc = curl_multi_socket_action(
      multiHandle, 
      fd, 
      action, 
      &running );

  curlVerify("curl_multi_socket_action from eventCallback", rc);

  eventer -> setRunning( running );

  scanMultiInfo( eventer );

  if ( eventer -> getRunning() <= 0 ) {
    qDebug() << "debug: last transfer complete";

    if (evtimer_pending( eventer -> getTimerEvent(), NULL)) {
      evtimer_del( eventer -> getTimerEvent() );
    }
  } else {
    qDebug() << "debug: " << eventer -> getRunning() << " live connections";
  }

}
 
void timerCallback(int fd, short kind, void* oEventer) 
{
  CURLMcode mrc;
  Eventer* eventer = (Eventer*) oEventer;
  int running;
  CURLM* multiHandle;

  (void)fd;
  (void)kind;

  qDebug() << "debug: calling curl_multi_socket_action on fd " << fd;

  /* FIXME: move this code to eventer::updateTimeout */
  multiHandle = eventer -> getMultiHandle();

  mrc = curl_multi_socket_action(
      multiHandle,
      CURL_SOCKET_TIMEOUT, 
      0, 
      &running );

  curlVerify("curl_multi_socket_action from timerCallback", mrc);

  eventer -> setRunning( running );

  qDebug() << "debug: in timerCallback, called curl_multi_socket_action recvd "
	    << ( eventer -> getRunning() ) << " live sockets";

  scanMultiInfo( eventer );
}

/* FIXME: move this function to the eventer */
void setSocket(
    Retriever* retriever, 
    curl_socket_t s, 
    CURL*e, 
    int act,  
    Eventer* eventer)
{
  CURLM* multiHandle;
  int kind =  ( act & CURL_POLL_IN ? EV_READ : false )
            | ( act & CURL_POLL_OUT ? EV_WRITE : false ) 
            | EV_PERSIST;

  qDebug() << "debug: in setSocket with socket " << s
           << "kind " << kind 
           << " easy@ " << e 
           << " act " << act
           << " eventer@ " << eventer
           << " retriever@ " << retriever;

  /* FIXME: implement this within eventer::updateSocket */

  multiHandle = eventer -> getMultiHandle();
  curl_multi_assign( multiHandle, s, retriever );
  retriever -> setSocketData( s, act, kind, e );
}

void addSocket(
    curl_socket_t s, 
    CURL *easy, 
    int action, 
    Eventer* eventer)
{
    Retriever* retriever = NULL;
    curl_easy_getinfo( easy, CURLINFO_PRIVATE, &retriever );
    setSocket( retriever, s, easy, action, eventer );
}


int multiTimerCallback(
        CURLM *multi_handle, 
	long timeout_ms,
	void *oEventer)
{
    struct timeval timeout;
    Eventer* eventer = ((Eventer*) oEventer );
    struct event* timerEvent = eventer -> getTimerEvent();
    struct event_base* eventBase = eventer -> getEventBase();

    (void)multi_handle;

    timeout.tv_sec = timeout_ms/1000;
    timeout.tv_usec = (timeout_ms%1000)*1000;

    qDebug() << "debug: in multi timer callback setting timeout to "
	      << timeout_ms << "ms with timerEvent @" 
	      << timerEvent << " and eventBase @"
	      << eventBase;

    /* FIXME: implement this within eventer::addTimer */

    if( evtimer_add( timerEvent, &timeout) == -1 ) {
        qFatal("error: evtimer_add(..) failed!\n");
    }

    return 0;
}

int socketCallback(
        CURL *e, 
	curl_socket_t s, 
	int what, 
	void *eventer, 
	void *retriever)
{
  const char *whatLut[] = { "none", "IN", "OUT", "INOUT", "REMOVE" };

  qDebug() << "debug: socket callback: socket " << s
	    << "easy handle: " << e 
	    << "event: " << whatLut[what] 
	    << "eventer @" << eventer
            << "retriever @" << retriever;

  if (what == CURL_POLL_REMOVE) {
      qDebug() << "debug: remove stub";
  } else {
    if (!retriever) {
      addSocket(s, e, what, (Eventer*) eventer); 
    } else {
      setSocket( (Retriever*) retriever, s, e, what, (Eventer*) eventer); 
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

  qDebug() << "debug: in write callback with ptr @" 
	   << ptr << " and size "
	   << size << " and nmemb " 
	   << nmemb << " and data @"
           << data; 

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
 qDebug() << "debug: in progress callback with ptr @" 
	   << p << " and dltotal " 
	   << dltotal << " and dlnow " 
	   << dlnow << " and ult " 
	   << ult << " and uln" 
	   << uln;

 return 0;
}

void keyboardCallback(
        evutil_socket_t s,
	short type, 
	void *data)
{
  Eventer* eventer = (Eventer*) data;
  QString userInput = "";
  char cchar;
  qDebug() << "debug: in keyboard callback with socket s "
	    << s << " type "
	    << type << " and data @"
	    << data;

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

/* FIXME: move into the eventer class */
void scanMultiInfo( Eventer* eventer )
{
  CURLMsg *msgPtr;
  CURLM *multi;
  CURL *easy;
  CURLcode rc;
  Retriever* retriever;
  char *URI;
  int msgsRemaining;

  qDebug() << "debug: remaining " << eventer -> getRunning();

  multi = eventer -> getMultiHandle();

  while ((msgPtr = curl_multi_info_read(multi, &msgsRemaining))) {
    if (msgPtr -> msg == CURLMSG_DONE) {

      easy = msgPtr -> easy_handle;
      rc = msgPtr -> data.result;

      curl_easy_getinfo(easy, CURLINFO_PRIVATE, &retriever);
      curl_easy_getinfo(easy, CURLINFO_EFFECTIVE_URL, &URI);

      qDebug() << "debug: " << URI
	        << " complete, rc = " << rc 
		<< " error buffer: " << 
		( retriever -> getErrorBuffer() );

      curl_multi_remove_handle(multi, easy);
      delete retriever;
    }
  }
}

} /* namespace slurp */
