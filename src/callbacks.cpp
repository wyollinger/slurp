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

using namespace slurp;

void slurp::curlVerify(const char *where, CURLMcode code)
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
      qDebug() << "debug: " << where << " returns " << s;
      /* ignore this error */ 
      return;
    }

    qDebug() << "debug: " << where << " returns " << s;
    exit(code);
  }
}

void slurp::eventCallback(int fd, short kind, void *userp)
{
  Eventer *eventer = (Eventer*) userp;
  CURLMcode rc;
  int action, running;

  qDebug() << "debug: in event callback with fd " 
	   << fd << " kind " 
	   << kind;

  action =
    (kind & EV_READ ? CURL_CSELECT_IN : 0) |
    (kind & EV_WRITE ? CURL_CSELECT_OUT : 0);

  rc = curl_multi_socket_action(
      eventer -> getMultiHandle(), 
      fd, 
      action, 
      &running );

  curlVerify("eventCallback: curl_multi_socket_action", rc);
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
 
void slurp::timerCallback(int fd, short kind, void* oEventer) 
{
  CURLMcode mrc;
  Eventer* eventer = (Eventer*) oEventer;
  int running;

  /* to remove unused warnings */
  (void)fd;
  (void)kind;

  qDebug() << "debug: calling curl_multi_socket_action on fd " << fd;

  mrc = curl_multi_socket_action(
      eventer -> getMultiHandle(),
      CURL_SOCKET_TIMEOUT, 
      0, 
      &running );

  curlVerify("timerCallback: curl_multi_socket_action", mrc);
  eventer -> setRunning( running );

  qDebug() << "debug: in timerCallback, called curl_multi_socket_action recvd "
	    << ( eventer -> getRunning() ) << " live sockets";

  scanMultiInfo( eventer );
}

void slurp::setSocket(
    Retriever* retriever, 
    curl_socket_t s, 
    CURL*e, 
    int act,  
    Eventer* eventer)
{
  int kind = ( act & CURL_POLL_IN ? EV_READ : false )
            | ( act & CURL_POLL_OUT ? EV_WRITE : false ) 
            | EV_PERSIST;

  (void) eventer;  

  retriever -> setSocketData( s, act, kind, e );
}

void slurp::addSocket(
    curl_socket_t s, 
    CURL *easy, 
    int action, 
    Eventer* eventer)
{
    Retriever* retriever = NULL;
    curl_easy_getinfo( easy, CURLINFO_PRIVATE, &retriever );

    qDebug() << "debug: in addSocket with socket " << s 
             << " easy handle@" << easy
             << " action " << action
             << " eventer@ " << eventer 
             << " retriever@ " << retriever;
   
    curl_multi_assign( eventer->getMultiHandle(), s, retriever);
     
    setSocket( retriever, s, easy, action, eventer );

    /* note: this will cause the retriever pointer to be valid in
       future calls to socketCallback */
}


int slurp::multiTimerCallback(
        CURLM * multi_handle, 
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

    if( evtimer_add( timerEvent, &timeout) == -1 ) {
        qFatal("error: evtimer_add(..) failed!\n");
    }

    return 0;
}

int slurp::socketCallback(
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
      /* delete (Retriever*) retriever; */
  } else {
    if (!retriever) {
      addSocket(s, e, what, (Eventer*) eventer); 
    } else {
      setSocket( (Retriever*) retriever, s, e, what, (Eventer*) eventer); 
    }
  }

  return 0;
}
 
size_t slurp::writeCallback(
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
 
int slurp::progressCallback(
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

void slurp::keyboardCallback(
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

  qDebug() << "debug: in kbcallback with <" << userInput.toAscii().data();

  if( userInput.size() > 0 ) {
     eventer -> queueURI( userInput );
  }

}


void slurp::scanMultiInfo( Eventer* eventer)
{
  CURLMsg *msgPtr;
  CURL *easy;
  CURLM *multi;
  CURLcode rc;
  Retriever* cRetriever;
  char *effectiveUri;
  int msgsRemaining;

  multi = eventer -> getMultiHandle();

  qDebug() << "debug: remaining " << eventer -> getRunning();

  while ((msgPtr = curl_multi_info_read(multi, &msgsRemaining))) {
   // if (msgPtr->msg == CURLMSG_DONE) {
      easy = msgPtr->easy_handle;
      rc = msgPtr -> data.result;
      curl_easy_getinfo(easy, CURLINFO_PRIVATE, &cRetriever);
      curl_easy_getinfo(easy, CURLINFO_EFFECTIVE_URL, &effectiveUri);

      qDebug() << "debug: " << effectiveUri 
	        << " complete, rc = " << rc 
		<< " error buffer: " << cRetriever->getErrorBuffer();

      curl_multi_remove_handle(multi, easy);
      delete cRetriever;
    //}
  }
}

