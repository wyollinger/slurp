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

#include <QDebug>

#include "eventer.h"
#include "callbacks.h"
#include "util.h"

namespace slurp {

Eventer::Eventer( 
    const QQueue<QString>& initialURIs, 
    int quota, 
    int flags ) {
      running = 0;

      this -> quota = quota;
      this -> flags = flags;
      this -> initialURIs = initialURIs;

      eventBasePtr = event_base_new();

      multi = curl_multi_init();

      timerEventPtr = event_new( eventBasePtr, -1, 0, timerCallback, this );

      curlVerify( "error: multi_setopt: socket callback", 
          curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, socketCallback));
      curlVerify( "error: multi_setopt: socket data",
	  curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, this));
      curlVerify( "error: multi_setopt: timer callback", 
	  curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, multiTimerCallback));
      curlVerify( "error: multi_setopt: timer data",
          curl_multi_setopt(multi, CURLMOPT_TIMERDATA, this));

      qDebug() << "debug: constructed eventer instance with multi @" 
	        << multi << " and timerEvent @"
		<< timerEventPtr << " and eventBase @"
		<< eventBasePtr << " with location @"
		<< this;
}

  
event* Eventer::registerSocket( curl_socket_t sockfd, int kind )
{
   event* newEvent;

   newEvent = event_new( eventBasePtr, sockfd, kind, eventCallback, this);

   return newEvent;
}

void Eventer::addHandle( CURL* handle ) {
  CURLMcode rc;

  rc = curl_multi_add_handle(multi, handle);

  curlVerify("curl_multi_add_handle from Retriever()", rc);
}

void Eventer::processSocketEvent( int fd, short kind ) {
  CURLMcode rc;
  int action =
    ( kind & EV_READ ? CURL_CSELECT_IN : 0 ) |
    ( kind & EV_WRITE ? CURL_CSELECT_OUT : 0 );
 
  rc = curl_multi_socket_action(
      multi, 
      fd, 
      action, 
      &running );

  curlVerify("curl_multi_socket_action from Eventer::processSocketEvent", rc);

}

void Eventer::checkTimer() {
  if ( running <= 0 ) {
    qDebug() << "debug: last transfer complete";

    if ( evtimer_pending( timerEventPtr, NULL)) {
      evtimer_del( timerEventPtr );
    }
  }
}

void Eventer::updateTimer() {
  CURLMcode mrc;

  mrc = curl_multi_socket_action(
      multi,
      CURL_SOCKET_TIMEOUT, 
      0, 
      &running );

  curlVerify("curl_multi_socket_action from timerCallback", mrc);
}

void Eventer::addTimer( long timeout_ms ) {
  struct timeval timeout;

  timeout.tv_sec = timeout_ms/1000;
  timeout.tv_usec = (timeout_ms%1000)*1000;

  if( evtimer_add( timerEventPtr, &timeout) == -1 ) {
      qFatal("error: evtimer_add(..) failed!\n");
  }
}

int Eventer::run() {
  int ret;
  event* kbEvent;

  while (!initialURIs.isEmpty()) {
      addURI( initialURIs.dequeue() ); 
  }

  kbEvent = event_new(
     eventBasePtr, 
     0, 
     EV_READ | EV_PERSIST, 
     keyboardCallback, this);

  if( event_add(kbEvent, NULL) == -1 ) {
    qFatal("error: could not add keyboard event\n");
    exit(1);
  } 

  qDebug() << "debug: calling event_base_dispatch";

  ret = event_base_dispatch( eventBasePtr );

  qDebug() << "debug: event dispatch returned " << ret;

  event_free( kbEvent );

  checkTimer();
  event_free( timerEventPtr );
  timerEventPtr = NULL;

  event_base_free( eventBasePtr );
  eventBasePtr = NULL;

  curl_multi_cleanup(multi);
  multi = NULL;

  qDebug() << "debug: Eventer::run() returning";
  return ret;
}

void Eventer::addURI( const QString& uri ) {
  new Retriever( this, uri, flags );
}

void Eventer::setSocket(
    Retriever* retriever, 
    curl_socket_t s, 
    CURL*e, 
    int act)
{
  int kind =  ( act & CURL_POLL_IN ? EV_READ : false )
            | ( act & CURL_POLL_OUT ? EV_WRITE : false ) 
            | EV_PERSIST;

  qDebug() << "debug: in setSocket with socket " << s
           << "kind " << kind 
           << " easy@ " << e 
           << " act " << act
           << " eventer@ " << this
           << " retriever@ " << retriever;

  curl_multi_assign( multi, s, retriever );

  retriever -> setSocketData( s, act, kind, e );
}

void Eventer::addSocket(
    curl_socket_t s, 
    CURL *easy, 
    int action )
{
    Retriever* retriever = NULL;
    curl_easy_getinfo( easy, CURLINFO_PRIVATE, &retriever );
    setSocket( retriever, s, easy, action);
}

void Eventer::scanMultiInfo()
{
  CURLMsg *msgPtr;
  CURL *easy;
  CURLcode rc;
  Retriever* retriever;
  char *URI;
  int msgsRemaining;

  while ((msgPtr = curl_multi_info_read(multi, &msgsRemaining))) {
    if (msgPtr -> msg == CURLMSG_DONE) {

      easy = msgPtr -> easy_handle;
      rc = msgPtr -> data.result;

      curl_easy_getinfo(easy, CURLINFO_PRIVATE, &retriever);
      curl_easy_getinfo(easy, CURLINFO_EFFECTIVE_URL, &URI);

      qDebug() << "debug: " << URI
	        << " complete, rc = " << rc 
		<< " error buffer: " << ( retriever -> getErrorBuffer() )
		<< " content buffer: " << retriever -> getData();

      curl_multi_remove_handle(multi, easy);
      delete retriever;
    }
  } /* while */

}

void Eventer::stop() {
  event_base_loopbreak( eventBasePtr );
}

} /* namespace slurp */
