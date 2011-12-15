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

#include "globals.h"
#include "eventer.h"
#include "util.h"
#include "parser.h"

namespace slurp {

    Eventer::Eventer(
        int argc, 
        char** argv,
        QQueue < QString > &initialUrls, 
        int quota, 
        int flags ) : QApplication( argc, argv, QApplication::GuiClient ) {
            QUrl currentUrl;

            setOrganizationName("Megafrock Laboratories");
            setApplicationName("Slurp");

            qRegisterMetaType<parseResult>("parseResult");

            this->quota = quota;
            this->flags = flags;

			foreach( QUrl currentUrl, initialUrls ) {
				emit addUrl( currentUrl );
			}

            QObject::connect( 
                this, SIGNAL(aboutToQuit()),
                this, SLOT(crawlFinished()) );

            active = false;
        }

    void Eventer::addUrl( QUrl url ) {
        if( !url.isValid() ) {
            qDebug() << "discarding invalid " << url;
            return;
        } 

        if( url.scheme() == "https" ) {
            qDebug() << "discarding https " << url;
            return;
        }

        if( visitedUrls.contains( url ) ) {
            qDebug() << "discarding duplicate" << url;
            return;
        } else {
			visitedUrls.insert( url );
		}

        if( url.host() == "" ) {
            qDebug() << "discarding url with no host";
            return;
        }

        qDebug() << "queuing a new parser";
        queuedParsers.enqueue( new Parser( url ) );

        emit statsChanged( queuedParsers.count(), visitedUrls.count() );

        if( active ) {
            emit dispatchParsers();
        }
    }


    void Eventer::dispatchParsers() {
        while( active && ! queuedParsers.isEmpty() && runningParsers.count() < 8 ) {
            qDebug() << "starting a new parser";
        
            Parser* queuedParser = queuedParsers.dequeue();
      
            emit queuedParser -> requestPage();

            QObject::connect(queuedParser, SIGNAL(finished(parseResult)), 
                this, SLOT(parserFinished(parseResult)));
                
            QObject::connect(queuedParser, SIGNAL(progress(int)),
                this, SLOT(parserProgress(int)));

            QObject::connect(this, SIGNAL(consumedUrls()),
                queuedParser, SLOT(cleanup()));

            runningParsers.push_back( queuedParser );
			
			qDebug() << " queued: " << queuedParsers.count();
        }
    }

    /* TODO: emit senderParser in signal */
    void Eventer::parserFinished( parseResult urls ) {
        Parser* senderParser = 
            reinterpret_cast< Parser* > ( QObject::sender() );
   
        qDebug() << " eventer got " << urls.count() << " urls";

        foreach( QUrl currentUrl, urls ) {
            emit addUrl( currentUrl );
            emit newUrl( currentUrl );
        }
    
        if( runningParsers.contains( senderParser ) ) {
            int i = runningParsers.indexOf( senderParser );
            runningParsers.erase( runningParsers.begin() + i );
        } else {
            qDebug() << " warning: got parserFinished() from an unknown source";
        }

        if( active ) {
            emit dispatchParsers(); 
        }

        emit consumedUrls();
        emit statsChanged( queuedParsers.count(), visitedUrls.count() );
    } 

    void Eventer::crawlFinished() {
        qDebug() << "slurp shutting down";
    }

    void Eventer::stopCrawling() {
        qDebug() << "user aborted crawl";

        queuedParsers.clear();
        active = false;
    }

    void Eventer::startCrawling() {
        active = true;
        emit dispatchParsers();
    }

    void Eventer::parserProgress( int n ) {
        emit progressChanged( n );
    }

}    /* namespace slurp */
