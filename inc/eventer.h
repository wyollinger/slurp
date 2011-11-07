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

#include <event2/event.h>
#include <event2/thread.h>
#include <event2/buffer.h>

#include "retriever.h"

namespace slurp {
  class Eventer {
     QQueue<QString> pendingURIs;
     QSet<QString> processedURIs;
     QThreadPool threadPool;

     int quota;

     public:

     Eventer( const QQueue<QString>& initialURIs, int quota, int maxThreads );
     int run();
     void queueURI( const QString& uri );
  };
}

#endif 



