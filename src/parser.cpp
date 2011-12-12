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

#include <QDebug>
#include <QObject>
#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>
#include <QUrl>

#include "parser.h"
#include "eventer.h"
#include "retriever.h"
#include "util.h"

namespace slurp {

    /* NOTE: parser does not delete it's page instance */

    Parser::Parser(Eventer * owner, QString raw_url, QString raw_data) {
        this->owner = owner;
        url = QUrl(raw_url);
        data = raw_data;
    } 
    
    void Parser::run() {
        setAutoDelete(false);

        qDebug() << "debug: in parse thread " << QThread::currentThreadId();

        qDebug() << "debug: eventer's parser pool insance at "
                 << owner -> getParserPool();

        qDebug() << "debug: constructing web page instance";
        page = new QWebPage();

        /*
         * Because setHtml can cause asynchronous behavior
         * related to page construction we must wait until
         * the signals are dispatched by the page, but because
         * this QRunnable has no event loop, we connect them
         * to the Threader instead and the threader then 
         * schedules the scan.
         */ 
        QObject::connect( 
            page, 
            SIGNAL(loadStarted()),
            owner -> getParserPool(),
            SLOT(loadStartedCallback()));

         QObject::connect( 
           page, 
           SIGNAL(loadProgress(int)),
           owner -> getParserPool(),
           SLOT(loadProgressCallback(int)));
         
        QObject::connect(
            page,
            SIGNAL(loadFinished(bool)),
            owner -> getParserPool(),
            SLOT(loadFinishedCallback(bool)));

        qDebug() << "debug: setting html with "
            << data.size() << " bytes of data..";
        page->mainFrame()->setHtml(data, url);
    }
}                               /* namespace slurp */
