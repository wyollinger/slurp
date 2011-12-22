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

#include <QString>
#include <QDebug>
#include <QThread>
#include <QApplication>
#include <QFile>
#include <QUrl>

#include "globals.h"
#include "eventer.h"
#include "interacter.h"

namespace slurp {

    const char *LICENSE_INFO =
        "Copyright (C) 2011 Joseph Max DeLiso\n\n"
        "This program is free software: you can redistribute it and/or modify\n"
        "it under the terms of the GNU General Public License as published by\n"
        "the Free Software Foundation, version 3 of the License.\n\n"
        "This program is distributed in the hope that it will be useful,\n"
        "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
        "GNU General Public License for more details.\n\n"
        "You should have received a copy of the GNU General Public License\n"
        "along with this program.\n"
        "If not, see <http://www.gnu.org/licenses/>.\n";

    const char *LOGFILE_NAME = "slurp.log";


} /* namespace slurp */ 

using namespace slurp;

int main(int argc, char **argv) {
    QQueue < QString > seedUrls;

    Eventer::logFile.setFileName( LOGFILE_NAME );
    Eventer::logFile.open( QIODevice::ReadWrite );

    qInstallMsgHandler( Eventer::debugHandler );
 
	qDebug() << "slurp started up";

    Eventer ev( argc, argv);

    Interacter inter;

    QObject::connect( &inter, SIGNAL( crawlClicked( QUrl ) ), 
                      &ev, SLOT( addUrl( QUrl ) ));

    QObject::connect( &inter, SIGNAL( crawlStarted() ),
                      &ev, SLOT( startCrawling() ) );

    QObject::connect( &inter, SIGNAL( crawlAborted() ),
                      &ev, SLOT( stopCrawling() ) );
   
    QObject::connect( &inter, SIGNAL( forceCrawlAbort() ),
                      &ev, SLOT( forceStop() ) );              
                      
    QObject::connect( &ev, SIGNAL( newUrl( QUrl ) ), 
                      &inter, SLOT( newUrl( QUrl ) ) );

    QObject::connect( &ev, SIGNAL( statsChanged(int, int) ),
                      &inter, SLOT( updateStats(int, int) ) );

    QObject::connect( &ev, SIGNAL( progressChanged(int) ),
                      &inter, SLOT( updateProgress(int) ) );

    QObject::connect( &ev, SIGNAL( lastParserFinished() ),
                      &inter, SLOT( stopComplete() ) );

    inter.show();

    return ev.exec();
}
