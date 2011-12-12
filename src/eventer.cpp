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
                currentUrl = QUrl( initialUrls.dequeue() );

                if( currentUrl.isValid() ) {
                    qDebug() << "debug: adding " << currentUrl << " to queued parser";
                    queuedParsers.enqueue( new Parser( this, currentUrl ) );
                }
            }

        }

    void Eventer::addUrl( QUrl url ) {
        
    }

}    /* namespace slurp */
