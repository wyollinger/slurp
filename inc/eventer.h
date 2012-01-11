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
#include <QFile>
#include <QProcess>

#include "globals.h"
#include "parser.h"

namespace slurp {

    class Parser;
    class Eventer : public QApplication {
        Q_OBJECT

        QQueue < QUrl > queuedUrls;
        QVector < QProcess* > runningParsers;
        QQueue < QProcess* > queuedParsers;
        QSet < QUrl > urlSet;
        QMap < QUrl , int > retryMap;

        bool active;

    public:

        Eventer(int& argc, char** argv);

        static QFile logFile; 
        static void die(const char *errmsg, int errcode );
        static void debugHandler(QtMsgType type, const char* msg);

    public slots:    

        void addUrl(QUrl url);
        //void parserFinished( parseResult urls, Parser* parser );
        void parserFinished (int exitCode, QProcess::ExitStatus exitStatus);
        void stopCrawling();
        void startCrawling();
        void handleParseFailure( QUrl url, Parser* parser );
        void forceStop();

    private slots:

        void dispatchParsers();
        void parserProgress( int n );
        void readParserData ();

    signals:
    
        void consumedUrls();
        void statsChanged( int queued, int crawled );
        void progressChanged( int n );
        void newUrl( QUrl url );
        void lastParserFinished();
    };

}   /* namespace slurp */

#endif  /* SLURP_EVENTER_H */
