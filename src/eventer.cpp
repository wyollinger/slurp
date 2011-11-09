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

/* #include "globals.h" */
#include "retriever.h"
#include "scanner.h"

using namespace slurp;


Eventer::Eventer( 
    const QQueue<QString>& initialURIs, 
    int quota, 
    int maxThreads,
    int flags ) {
      running = 0;

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

      std::cout << "debug: constructed eventer instance with multi @" 
	        << multi << " and timerEvent @"
		<< timerEventPtr << " and eventBase @"
		<< eventBasePtr << " with location @"
		<< this << "\n";
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
  struct event* kbEvent;

  /* create or schedule thread creation for every pending inital request. */
  while (!pendingURIs.isEmpty()) {
      threadPool.start( new Retriever( this, pendingURIs.dequeue(), flags ) );
  }

  /* 
   * need to add some kind of event here so that the event loop
   * can begin.
   *
   */

  kbEvent = event_new(
     eventBasePtr, 
     0, 
     EV_READ | EV_PERSIST, 
     Eventer::keyboardCallback, this);

  if( event_add(kbEvent, NULL) == -1 ) {
    std::cerr << "error: could not add keyboard event\n";
  } 

  std::cout << "debug: calling event_base_dispatch\n";
  ret = event_base_dispatch( eventBasePtr );
  std::cout << "debug: event dispatch returned " << ret << "\n";

  event_free( kbEvent );

  return ret;
}

void Eventer::queueURI( const QString& uri ) {
  pendingURIs.enqueue( uri );
}

void Eventer::eventCallback(int fd, short kind, void *userp)
{
  Eventer *eventer = (Eventer*) userp;
  CURLMcode rc;
  int action, running;

  std::cout << "debug: in event callback with fd " 
	   << fd << " kind " 
	   << kind << "\n";

  action =
    (kind & EV_READ ? CURL_CSELECT_IN : 0) |
    (kind & EV_WRITE ? CURL_CSELECT_OUT : 0);

  rc = curl_multi_socket_action(
      eventer -> getMultiHandle(), 
      fd, 
      action, 
      &running );

  Eventer::curlVerify("eventCallback: curl_multi_socket_action", rc);
  eventer -> setRunning( running );

  Eventer::scanMultiInfo( eventer );

  if ( eventer -> getRunning() <= 0 ) {
    std::cout << "debug: last transfer complete\n";
    if (evtimer_pending( eventer -> getTimerEvent(), NULL)) {
      evtimer_del( eventer -> getTimerEvent() );
    }
  }
}
 
void Eventer::timerCallback(int fd, short kind, void* oEventer) 
{
  CURLMcode mrc;
  Eventer* eventer = (Eventer*) oEventer;
  int running;

  /* to remove unused warnings */
  (void)fd;
  (void)kind;

  mrc = curl_multi_socket_action(
      eventer -> getMultiHandle(),
      CURL_SOCKET_TIMEOUT, 
      0, 
      &running );

  Eventer::curlVerify("timerCallback: curl_multi_socket_action", mrc);
  eventer -> setRunning( running );

  std::cout << "debug: in timerCallback, called curl_multi_socket_action recvd "
	    << ( eventer -> getRunning() ) << " live sockets\n";

  scanMultiInfo( eventer );
}

int Eventer::multiTimerCallback(
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

    std::cout << "debug: in multi timer callback setting timeout to "
	      << timeout_ms << "ms with timerEvent @" 
	      << timerEvent << " and eventBase @"
	      << eventBase << "\n";

    if( evtimer_add( timerEvent, &timeout) == -1 ) {
        std::cerr << "error: evtimer_add(..) failed!\n";
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
  Eventer* eventer = (Eventer*) cbp;
  /* SockInfo *fdp = (SockInfo*) sockp; */
  const char *whatLut[] = { "none", "IN", "OUT", "INOUT", "REMOVE" };

  std::cout << "debug: socket callback: socket " << s
	    << "easy handle: " << e 
	    << "event: " << whatLut[what] 
	    << "and eventer @" << eventer;

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
 std::cout << "debug: in write callback with ptr @" 
	   << ptr << " and size "
	   << size << " and nmemb " 
	   << nmemb << " and data @"
           << data << "\n"; 

 return 0;
}
 
int Eventer::progressCallback(
	void *p, 
	double dltotal, 
	double dlnow, 
	double ult,
        double uln)
{
 std::cout << "debug: in progress callback with ptr @" 
	   << p << " and dltotal " 
	   << dltotal << " and dlnow " 
	   << dlnow << " and ult " 
	   << ult << " and uln" 
	   << uln << "\n";

 return 0;
}

void Eventer::keyboardCallback(
        evutil_socket_t s,
	short type, 
	void *data)
{
  std::cout << "debug: in keyboard callback with socket s "
	    << s << " type "
	    << type << " and data @"
	    << data << "\n";

  while( std::cin.get() != '\n' );
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

  std::cout << "debug: remaining " << eventer -> getRunning() << "\n";

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

