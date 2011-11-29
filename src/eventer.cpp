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
#include "callbacks.h"

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
		<< this << "\n";
}

Eventer::~Eventer() {
  threadPool.waitForDone();
  event_free( timerEventPtr );
  event_base_free( eventBasePtr );
  curl_multi_cleanup(multi);
}

int Eventer::run() {
  int ret;
  struct event* kbEvent;

  /* create or schedule thread creation for every pending inital request. */
  while (!pendingURIs.isEmpty()) {
      queueURI( pendingURIs.dequeue() ); 
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

  qDebug() << "debug: calling event_base_dispatch\n";
  ret = event_base_dispatch( eventBasePtr );
  qDebug() << "debug: event dispatch returned " << ret << "\n";

  event_free( kbEvent );

  return ret;
}

void Eventer::queueURI( const QString& uri ) {
  threadPool.start( new Retriever( this, uri, flags ) );
}
