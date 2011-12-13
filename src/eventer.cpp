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

#include <QApplication>
#include <QCoreApplication>
#include <QMutex>
#include <QDebug>
#include <QUrl>
#include <QQueue>

#include "eventer.h"
#include "util.h"
#include "parser.h"

namespace slurp {

    Eventer::Eventer(
        int argc, 
        char** argv,
        QQueue < QString > &initialUrls, 
        int quota, 
        int flags,
        int maxThreads ) : QApplication( argc, argv ) {
            QUrl currentUrl;

            this->quota = quota;
            this->flags = flags;
            this->maxThreads = maxThreads;

            while( !initialUrls.isEmpty() ) {
                addUrl( initialUrls.dequeue());
            }
        }

    void Eventer::addUrl( QUrl url ) {
        if( !url.isValid() ) {
            qDebug() << "warning: discarding invalid " << url;
            return;
        } 

        queueMutex.lock();
        queuedParsers.enqueue( new Parser( this, url ) );

        runMutex.lock();
        bool done = false;
        for( int i = runningParsers.count();
             !done && i < maxThreads;
             ++i ) {
            qDebug() << " i = " << i << " max = " << maxThreads;

            if( !queuedParsers.isEmpty() ) {
                Parser* currentParser = queuedParsers.dequeue();
                currentParser->start();
                runningParsers.push_back( currentParser );
            } else {
                done = true;
            }
        }
        runMutex.unlock();

        queueMutex.unlock();
    }

}    /* namespace slurp */
