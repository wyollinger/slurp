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
#include <QVector>
#include <QMetaType>
#include <QSet>

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
        int maxThreads ) : QApplication( argc, argv, QApplication::GuiClient ) {
            QUrl currentUrl;

            setOrganizationName("Megafrock Laboratories");
            setApplicationName("Slurp");

            qRegisterMetaType<parseResult>("parseResult");

            this->quota = quota;
            this->flags = flags;
            this->maxThreads = maxThreads;

            while( !initialUrls.isEmpty() ) {
                addUrl( initialUrls.dequeue() );
            }
        }

    void Eventer::addUrl( QUrl url ) {

        if( !url.isValid() ) {
            qDebug() << "warning: discarding invalid " << url;
            return;
        } 

        if( url.scheme() == "https" ) {
            qDebug() << "information: discarding https url " << url;
            return;
        }

        visitedMutex.lock();
        if( visitedUrls.contains( url ) ) {
            qDebug() << "information: discarding url because it's already been visited";
            visitedMutex.unlock();
            return;
        }
        visitedUrls.insert( url );
        visitedMutex.unlock();

        qDebug() << thread() << "waiting for queue mutex";
        queueMutex.lock();

        qDebug() << thread() << "queuing a new parser";
        queuedParsers.enqueue( new Parser( this, url ) );

        queueMutex.unlock();
        emit dispatchParsers();
    }


    void Eventer::dispatchParsers() {
        qDebug() << thread() << "waiting for run lock";

        runMutex.lock();
            while( ! queuedParsers.isEmpty() && runningParserThreads.count() < maxThreads ) {
                qDebug() << thread() << "starting a new parser";

                QThread* newThread = new QThread();
                Parser* queuedParser = queuedParsers.dequeue();

                queuedParser -> moveToThread( newThread );

                QObject::connect(newThread, SIGNAL(started()),
                                 queuedParser, SLOT(requestPage()));

                QObject::connect(queuedParser, SIGNAL(finished(parseResult)), 
                                 this, SLOT(parserFinished(parseResult)));
                
                QObject::connect(this, SIGNAL(consumedUrls()),
                                 queuedParser, SLOT(cleanup()));

                newThread -> start();
                runningParserThreads.push_back( newThread );
                runningParserMap[ queuedParser ] = newThread;
            }
        runMutex.unlock();

        qDebug() << thread() << "released run lock";
    }

    void Eventer::parserFinished( parseResult urls ) {
        Parser* senderParser = reinterpret_cast< Parser* > ( QObject::sender() );
        QThread* senderThread = runningParserMap.take( senderParser );

        qDebug() << thread() << " eventer has " << urls.count() << " urls";

        foreach( QUrl currentUrl, urls ) {
            emit addUrl( currentUrl );
        }
    
        qDebug() << thread() << " waiting for run lock";
        runMutex.lock();
        
        if( runningParserThreads.contains( senderThread ) ) {
            int i = runningParserThreads.indexOf( senderThread );
            runningParserThreads.erase( runningParserThreads.begin() + i );
        } else {
            qDebug() << thread() << " warning: got parserFinished() from an unknown source";
        }

        runMutex.unlock();
        qDebug() << thread() << " released run lock";

        qDebug() << thread() << " source was " << senderParser << 
                              " on thread " << senderThread;

        senderThread->quit();

        emit dispatchParsers();
        emit consumedUrls();
    } 

}    /* namespace slurp */
