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
        }

    void Eventer::addUrl( QUrl url ) {
        if( !url.isValid() ) {
            return;
        } 

        if( url.scheme() == "https" ) {
            return;
        }

        if( visitedUrls.contains( url ) ) {
            return;
        } else {
			visitedUrls.insert( url );
		}

        qDebug() << thread() << "queuing a new parser";
        queuedParsers.enqueue( new Parser( url ) );

        emit dispatchParsers();
    }


    void Eventer::dispatchParsers() {
        while( ! queuedParsers.isEmpty() && runningParsers.count() < 8 ) {
            qDebug() << "starting a new parser";
        
            Parser* queuedParser = queuedParsers.dequeue();
      
            emit queuedParser -> requestPage();

            QObject::connect(queuedParser, SIGNAL(finished(parseResult)), 
                this, SLOT(parserFinished(parseResult)));
                
            QObject::connect(this, SIGNAL(consumedUrls()),
                queuedParser, SLOT(cleanup()));

            runningParsers.push_back( queuedParser );
			
			qDebug() << "parsed: " << visitedUrls.count() 
			         << " queued: " << queuedParsers.count()
					 << " parsing: " << runningParsers.count();
        }
    }

    void Eventer::parserFinished( parseResult urls ) {
        Parser* senderParser = reinterpret_cast< Parser* > ( QObject::sender() );
   
        qDebug() << " eventer got " << urls.count() << " urls";

        foreach( QUrl currentUrl, urls ) {
            emit addUrl( currentUrl );
        }
    
        if( runningParsers.contains( senderParser ) ) {
            int i = runningParsers.indexOf( senderParser );
            runningParsers.erase( runningParsers.begin() + i );
        } else {
            qDebug() << " warning: got parserFinished() from an unknown source";
        }

        emit dispatchParsers();
        emit consumedUrls();
    } 

}    /* namespace slurp */
