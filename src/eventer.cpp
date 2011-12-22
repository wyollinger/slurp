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
#include <QFile>

#include "globals.h"
#include "eventer.h"
#include "parser.h"

namespace slurp {

    QFile Eventer::logFile; 

    Eventer::Eventer(
        int& argc, 
        char** argv) : QApplication( argc, argv, true ) {
            QUrl currentUrl;

            setOrganizationName("Megafrock Laboratories");
            setApplicationName("Slurp");

            /* For an explanation on what this is doing and
             * why it is needed, you need to understand Qt's 
             * meta object compiler. 
             */
            qRegisterMetaType<parseResult>("parseResult");

            active = false;
        }

    void Eventer::die(const char *errmsg, int errcode) {
        qFatal(errmsg);
        exit(errcode);
    }

    void Eventer::debugHandler(QtMsgType type, const char* msg ) {
        (void)type;

        logFile.write( msg, qstrlen( msg ) );
        logFile.write( "\n", 1 );
    }

    void Eventer::addUrl( QUrl url ) {
        if( retryMap.contains( url ) && retryMap[ url ] >= 3 ) {
            qDebug() << "discarding url because we've failed to parse it thrice" 
                     << url;
            emit dispatchParsers();
            return;
        } 
        
        if( queuedUrls.contains( url ) && !retryMap.contains( url) ) {
            qDebug() << "discarding duplicate not in retry map" << url;
            return;
        }
         
		queuedUrls.insert( url );
		
        queuedParsers.enqueue( new Parser( url ) );

        emit statsChanged( queuedParsers.count(), queuedUrls.count() );
        emit dispatchParsers();
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

        emit dispatchParsers(); 
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

    void Eventer::stopCrawling() {
        qDebug() << "user aborted crawl";
        active = false;
    }

    void Eventer::startCrawling() {
        active = true;

        runningParsers.clear();
        retryMap.clear();

        emit dispatchParsers();
    }

    void Eventer::dispatchParsers() {
        if( !active ) {
            return;
        }

        while( !queuedParsers.isEmpty() && runningParsers.count() < 8 ) {
            qDebug() << "starting a new parser";
        
            Parser* queuedParser = queuedParsers.dequeue();
      
            emit queuedParser -> requestPage();

            QObject::connect(queuedParser, SIGNAL(finished(parseResult, Parser*)), 
                this, SLOT(parserFinished(parseResult, Parser*)));
                
            QObject::connect(queuedParser, SIGNAL(progress(int)),
                this, SLOT(parserProgress(int)));

            QObject::connect(queuedParser, SIGNAL(parseFailed(QUrl, Parser*)),
                this, SLOT(handleParseFailure(QUrl, Parser*)));
                
            QObject::connect(this, SIGNAL(consumedUrls()),
                queuedParser, SLOT(cleanup()));

            runningParsers.push_back( queuedParser );
			
			qDebug() << "queued: " << queuedParsers.count();
        }

        if( active && 
            runningParsers.count() == 0 && 
            queuedParsers.count() == 0 ) {
                qDebug() << "eventer: in dispatch parsers with"
                         << "nothing running and nothing queued";
                active = false;
                emit lastParserFinished();
        }
    }
    
    void Eventer::parserProgress( int n ) {
        emit progressChanged( n );
    }

    void Eventer::handleParseFailure( QUrl url, Parser* parser ) {
        if( !retryMap.contains( url ) ) {
            retryMap[ url ] = 1;
        } else {
            ++retryMap[ url ];
        }

        qDebug() << url << " has failed to parse " 
                 << retryMap[url] << " times";

        if( runningParsers.contains( parser ) ) {
            runningParsers.erase( 
                runningParsers.begin() +
                runningParsers.indexOf( parser ) );
            delete parser;
        } else {
            qDebug() << "warning: got parse failure for a parser not running";
        }

        emit addUrl( url );
        emit dispatchParsers();
    }

    void Eventer::forceStop() {
        qDebug() << "forcing stop due to timeout...\n";

        foreach( Parser* parser, runningParsers ) {
            emit parser->reset();
        }
    }
}    /* namespace slurp */
