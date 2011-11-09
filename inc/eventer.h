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

#ifndef EVENTER_H
#define EVENTER_H

#include <QString>
#include <QSet>
#include <QThreadPool>
#include <QThread>
#include <QRunnable>
#include <QMutex>
#include <QQueue>
#include <QListIterator>

#include <event2/event.h>
#include <event2/thread.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/event-config.h>
#include <event2/event_struct.h>

#include <curl/curl.h>

#include <cstdlib>
#include <cassert>

/* #include "globals.h" */
#include "retriever.h"
#include "scanner.h"

namespace slurp {
  class Eventer {
     QQueue<QString> pendingURIs;
     QSet<QString> processedURIs;
     QThreadPool threadPool;
     QMutex uriQueueMutex;
     CURLM *multi;
     int quota, flags, running;
     struct event_base* eventBasePtr;
     struct event* timerEventPtr;

     public:

     Eventer( 
         const QQueue<QString>& initialURIs, 
	 int quota, 
	 int maxThreads, 
	 int flags );
     ~Eventer();

     inline CURLM* getMultiHandle() const {
          return multi;
     }

     inline struct event* getTimerEvent() const {
          return timerEventPtr;
     }

     inline struct event_base* getEventBase() const {
          return eventBasePtr;
     }

     static void curlVerify(const char *where, CURLMcode code);
     static void eventCallback(int fd, short kind, void *userp);
     static void timerCallback(int fd, short kind, void* oEventer);
     static int multiTimerCallback(
        CURLM * multi_handle, 
	long timeout_ms, 
	void* oEventer);
     static int socketCallback(
        CURL *e, 
	curl_socket_t s, 
	int what, 
	void *cbp, 
	void *sockp);
     static size_t writeCallback(
        void *ptr, 
	size_t size, 
	size_t nmemb, 
	void *data);
     static int progressCallback(
	void *p, 
	double dltotal, 
	double dlnow, 
	double ult,
        double uln);
     static void keyboardCallback(
        evutil_socket_t s,
	short type, 
	void *data);

     static void scanMultiInfo( Eventer* eventer );

     int run();
     void queueURI( const QString& uri );
     int getRunning() const {
         return running;
     }
     void setRunning(int running) {
         this -> running = running; 
     }
  };
}


#endif 



