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

#ifndef SLURP_EVENTER_H
#define SLURP_EVENTER_H

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

#include "globals.h" 

namespace slurp {
  class Eventer {
     QQueue<QString> pendingURIs;
     QSet<QString> processedURIs;
     QThreadPool threadPool;
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

     inline int getRunning() const {
         return running;
     }

     inline void setRunning(int running) {
         this -> running = running; 
     }

     int run();

     void queueURI( const QString& uri );
  };
}

#endif /* SLURP_EVENTER_H */



