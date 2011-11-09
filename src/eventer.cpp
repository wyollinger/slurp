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
      eventBasePtr = event_base_new();

      multi = curl_multi_init();

      timerEventPtr = event_new( eventBasePtr, -1, 0, timerCallback, this );

      curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, socketCallback);
      curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, this);
      curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, multiTimerCallback);
      curl_multi_setopt(multi, CURLMOPT_TIMERDATA, this);
}

Eventer::~Eventer() {
  threadPool.waitForDone();
  event_free( timerEventPtr );
  event_base_free( eventBasePtr );
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

  /* create or schedule thread creation for every pending inital request. */
  while (!pendingURIs.isEmpty()) {
      threadPool.start( new Retriever( this, pendingURIs.dequeue(), flags ) );
  }

  /* start the event loop */
  ret = event_base_loop( eventBasePtr, 0 );

  std::cout << "debug: event loop returned " << ret << "\n";

  return ret;
}

void Eventer::queueURI( const QString& uri ) {
  pendingURIs.enqueue( uri );
}

void Eventer::eventCallback(int fd, short kind, void *userp)
{
  Eventer *oEventer = (Eventer*) userp;
  CURLMcode rc;
  int action, running;

  std::cout << "debug: in event callback with fd " 
	   << fd << " kind " 
	   << kind << "\n";

  action =
    (kind & EV_READ ? CURL_CSELECT_IN : 0) |
    (kind & EV_WRITE ? CURL_CSELECT_OUT : 0);

  rc = curl_multi_socket_action(
      oEventer -> getMultiHandle(), 
      fd, 
      action, 
      &running);

  Eventer::curlVerify("eventCallback: curl_multi_socket_action", rc);
  Eventer::scanMultiInfo( oEventer );

  if ( running <= 0 ) {
    std::cout << "debug: last transfer complete\n";
    if (evtimer_pending(oEventer->getTimerEvent(), NULL)) {
      evtimer_del(oEventer->getTimerEvent());
    }
  }
}
 
void Eventer::timerCallback(int fd, short kind, void* oEventer) 
{
  CURLMcode mrc;
  CURLcode rc;
  int running;

  mrc = curl_multi_socket_action(
      ((Eventer*) oEventer ) -> getMultiHandle(),
      CURL_SOCKET_TIMEOUT, 
      0, 
      &running);

  Eventer::curlVerify("timerCallback: curl_multi_socket_action", mrc);

  std::cout << "debug: in timerCallback, called curl_multi_socket_action recvd "
	    << running << " live sockets\n";

  scanMultiInfo( (Eventer*) oEventer );
}

int Eventer::multiTimerCallback(
        CURLM * multi_handle, 
	long timeout_ms,
	void *oEventer)
{
    struct timeval timeout;

    timeout.tv_sec = timeout_ms/1000;
    timeout.tv_usec = (timeout_ms%1000)*1000;

    std::cout << "debug: in multi timer callback setting timeout to "
	      << timeout_ms << "ms\n";

    if( evtimer_add( ((Eventer*)oEventer) -> getTimerEvent(), &timeout) ) {
        std::cerr << "evtimer_add(..) failed!\n";
    }

    return 0;
}

int Eventer::socketCallback(
        CURL *e, 
	curl_socket_t s, 
	int what, 
	void *cbp, 
	void *sockp)
{
  Eventer* oEventer = (Eventer*) cbp;
  /* SockInfo *fdp = (SockInfo*) sockp; */
  const char *whatLut[] = { "none", "IN", "OUT", "INOUT", "REMOVE" };

  std::cout << "debug: socket callback: socket " << s
	    << "easy handle: " << e 
	    << "event: " << whatLut[what] << "\n";

  if (what == CURL_POLL_REMOVE) {
      std::cout << "debug: stub: remove socket\n";
       /* remsock(fdp); */
  } else {
    if (!sockp) {
      std::cout << "debug: stub: add socket\n";
      /* addsock(s, e, what, g); */
    }
    else {
      std::cout << "debug: stub: changing actions\n";	    
      /* setsock(fdp, s, e, what, g); */
    }
  }

  return 0;
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

void Eventer::scanMultiInfo( Eventer* eventer)
{
  CURLMsg *msgPtr;
  CURL *easy;
  CURLM *multi;
  CURLcode rc;
  Retriever* cRetriever;
  char *effectiveUri;
  int msgsRemaining;

  multi = eventer -> getMultiHandle();

  while ((msgPtr = curl_multi_info_read(multi, &msgsRemaining))) {
    if (msgPtr->msg == CURLMSG_DONE) {
      easy = msgPtr->easy_handle;
      rc = msgPtr -> data.result;
      curl_easy_getinfo(easy, CURLINFO_PRIVATE, &cRetriever);
      curl_easy_getinfo(easy, CURLINFO_EFFECTIVE_URL, &effectiveUri);

      std::cout << "debug: " << effectiveUri 
	        << " complete, rc = " << rc 
		<< " error buffer: " << cRetriever->getErrorBuffer() 
		<< "\n";

      curl_multi_remove_handle(multi, easy);
      delete cRetriever;
    }
  }
}

