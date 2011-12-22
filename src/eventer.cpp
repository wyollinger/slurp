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
        int& argc, 
        char** argv,
        QQueue < QString > &initialUrls, 
        int quota, 
        int flags ) : QApplication( argc, argv, true ) {
            QUrl currentUrl;

            setOrganizationName("Megafrock Laboratories");
            setApplicationName("Slurp");

            /* For an explanation on what this is doing and
             * why it is needed, you need to understand Qt's 
             * meta object compiler. 
             */
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
        if( retryMap.contains( url ) && retryMap[ url ] >= 3 ) {
            qDebug() << "discarding url because we've failed to parse it thrice" 
                     << url;
            return;
        } else if( queuedUrls.contains( url ) ) {
            qDebug() << "discarding duplicate" << url;
            return;
        }
         
		queuedUrls.insert( url );
		
        queuedParsers.enqueue( new Parser( url ) );

        emit statsChanged( queuedParsers.count(), queuedUrls.count() );

        if( active ) {
            emit dispatchParsers();
        }
    }

    void Eventer::parserFinished( parseResult urls, Parser* parser ) {
        Parser* senderParser = 
            reinterpret_cast< Parser* > ( QObject::sender() );
   
        if( senderParser != parser ) {
            qDebug() << "warning: parser sender and object sender mismatch";
        }

        qDebug() << "eventer got " << urls.count() << " urls";

        foreach( QUrl currentUrl, urls ) {
            emit addUrl( currentUrl );
            emit newUrl( currentUrl );
        }
    
        if( runningParsers.contains( senderParser ) ) {
            runningParsers.erase( 
                runningParsers.begin() +
                runningParsers.indexOf( senderParser ) );

            delete parser;
        } else {
            qDebug() << "warning: got parserFinished() from an unknown source";
        }

        if( active ) {
            emit dispatchParsers(); 
        }

        emit consumedUrls();
        emit statsChanged( queuedParsers.count(), queuedUrls.count() );

        if( runningParsers.count() == 0 ) {
            emit lastParserFinished();
            qDebug() << "last parser complete";
        } else {
            qDebug() << "there are now " 
                     << runningParsers.count() 
                     << " running parsers";
        }
    } 

    void Eventer::crawlFinished() {
        qDebug() << "slurp shutting down";
    }

    void Eventer::stopCrawling() {
        qDebug() << "user aborted crawl";
        active = false;
    }

    void Eventer::startCrawling() {
        active = true;
        emit dispatchParsers();
    }

    void Eventer::dispatchParsers() {
        while( active && ! queuedParsers.isEmpty() && runningParsers.count() < 8 ) {
            qDebug() << "starting a new parser";
        
            Parser* queuedParser = queuedParsers.dequeue();
      
            emit queuedParser -> requestPage();

            QObject::connect(queuedParser, SIGNAL(finished(parseResult, Parser*)), 
                this, SLOT(parserFinished(parseResult, Parser*)));
                
            QObject::connect(queuedParser, SIGNAL(progress(int)),
                this, SLOT(parserProgress(int)));

            QObject::connect(queuedParser, SIGNAL(parseFailed(QUrl)),
                this, SLOT(handleParseFailure(QUrl)));
                
            QObject::connect(this, SIGNAL(consumedUrls()),
                queuedParser, SLOT(cleanup()));

            runningParsers.push_back( queuedParser );
			
			qDebug() << "queued: " << queuedParsers.count();
        }
    }
    
    void Eventer::parserProgress( int n ) {
        emit progressChanged( n );
    }

    void Eventer::handleParseFailure( QUrl url ) {
        if( !retryMap.contains( url ) ) {
            retryMap[ url ] = 1;
        } else {
            ++retryMap[ url ];
        }

        qDebug() << url << " has failed to parse " 
                 << retryMap[url] << " times";

        emit addUrl( url );
        emit dispatchParsers();
    }

    void Eventer::forceStop() {
        foreach( Parser* parser, runningParsers ) {
            emit parser->reset();
        }
    }
}    /* namespace slurp */
