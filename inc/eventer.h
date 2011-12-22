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
#include <QQueue>
#include <QVector>
#include <QThread>
#include <QMap>
#include <QApplication>
#include <QUrl>
#include <QSet>

#include "globals.h"
#include "parser.h"

namespace slurp {

    class Parser;
    class Eventer : public QApplication {
        Q_OBJECT

        QQueue < Parser* > queuedParsers;
        QVector < Parser* > runningParsers;
        QSet < QUrl > queuedUrls;
        QMap < QUrl , int > retryMap;

        int quota, flags;
        bool active;

    public:

        Eventer(int& argc, 
                char** argv,
                QQueue < QString > &initialUrls, 
                int quota, 
                int flags);

    public slots:    

        void addUrl(QUrl url);
        void parserFinished( parseResult urls, Parser* parser );
        void crawlFinished();
        void stopCrawling();
        void startCrawling();
        void handleParseFailure( QUrl url );
        
    private slots:

        void dispatchParsers();
        void parserProgress( int n );

    signals:
    
        void consumedUrls();
        void statsChanged( int queued, int crawled );
        void progressChanged( int n );
        void newUrl( QUrl url );
    };

}   /* namespace slurp */

#endif  /* SLURP_EVENTER_H */
