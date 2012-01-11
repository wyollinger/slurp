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
            /*Queue up all the QProcess's and do all the signal/slot connections*/
            QProcess* proc;
            for(int i=0; i<8; ++i){
                qDebug() << "Setting up Process " << i << endl;
                proc = new QProcess(this);
                QObject::connect(proc, SIGNAL(readyRead()), this, SLOT(readParserData()));
                QObject::connect(proc, SIGNAL( finished(int, QProcess::ExitStatus) ), this, SLOT( parserFinished(int, QProcess::ExitStatus) ));
                queuedParsers.enqueue(proc);
            }
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
        
        if( urlSet.contains( url ) && !retryMap.contains( url) ) {
            qDebug() << "discarding duplicate not in retry map" << url;
            return;
        }
        qDebug() << "Inserting and enqueueing url: " << url.toString() << endl;
         
		urlSet.insert( url );
		
        queuedUrls.enqueue( url );
        
        emit newUrl(QUrl(url));
        emit statsChanged( queuedUrls.count(), urlSet.count() );
        emit dispatchParsers();
    }
    void Eventer::readParserData(){
        QProcess* proc = qobject_cast<QProcess *>(QObject::sender());
        QByteArray ba;
        QString url;
        qDebug() << "Actually in readParserData so that's good" << endl;
        for(ba = proc->readLine(); !ba.isEmpty(); ba = proc->readLine()){
            if(ba.startsWith("URL: ")){
                qDebug() << "Got formatted URL: " << ba << endl;
                url = ba.mid(5);
                emit addUrl(QUrl(url));
            }
            else{
                qDebug() << "Got output that wasn't a URL\n\t " << ba << endl;
                //TODO: Make this signal and handle parser error
                //emit parserFailed();
            }
        }
        qDebug() << "Totally just read some parser data " << endl;
        
    }
    void Eventer::parserFinished(int exitCode, QProcess::ExitStatus status){
        if(exitCode==1 || status==QProcess::CrashExit){
            //emit parserFailed();
            //TODO: Make this signal and handle parser error
            return ;
        }
        QProcess * senderParser = qobject_cast<QProcess *>(QObject::sender());
        if(runningParsers.contains(senderParser)){
            runningParsers.erase(
                    runningParsers.begin()+
                    runningParsers.indexOf( senderParser ) );
        }
        else{
            qDebug() << "warning: got parserFinished() from an unknown source";
        }
        queuedParsers.enqueue(senderParser);
        qDebug() << "There are now " << queuedParsers.size() << " queued parsers" << endl;
        emit dispatchParsers();
        qDebug() << "there are now "
                << runningParsers.count()
                << " running parsers";

    }
    /*
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
            //Start Messing with things I don't understand yet
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
*/
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
        qDebug() << "Actually in dispatchParsers which is active " << active << endl;
        if( !active ) {
            return;
        }

        while( !queuedUrls.isEmpty() && runningParsers.count() < 8 ) {
            qDebug() << "starting a new parser";
        
            QProcess* queuedParser = queuedParsers.dequeue();
            QUrl url = queuedUrls.dequeue();
            QStringList args; //Eventually may be used to pass on options to the Parser program
            args << url.toString();

            queuedParser->start("./parser/bin/parser", args);
            qDebug() << "runing process on url: " << url.toString() << " with state " << queuedParser->state() << endl;

            /*QObject::connect(queuedParser, SIGNAL(finished(parseResult, Parser*)), 
                this, SLOT(parserFinished(parseResult, Parser*)));
                
            QObject::connect(queuedParser, SIGNAL(progress(int)),
                this, SLOT(parserProgress(int)));

            QObject::connect(queuedParser, SIGNAL(parseFailed(QUrl, Parser*)),
                this, SLOT(handleParseFailure(QUrl, Parser*)));
                
            QObject::connect(this, SIGNAL(consumedUrls()),
                queuedParser, SLOT(cleanup()));
*/
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
        //TODO: Change parameter list and write new function body to handle process failures
      /*  if( !retryMap.contains( url ) ) {
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
        } else {
            qDebug() << "warning: got parse failure for a parser not running";
        }

        emit addUrl( url );
        emit dispatchParsers();
        */
    }

    void Eventer::forceStop() {
        //TODO: Fix this so it kills the remaining parser processes
        qDebug() << "forcing stop due to timeout...\n";
/*
        foreach( Parser* parser, runningParsers ) {
            emit parser->reset();
        }
        */
    }
}    /* namespace slurp */
