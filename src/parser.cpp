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

    Parser::Parser(Eventer * owner, QString raw_url, QString raw_data) {
        this->owner = owner;
        url = QUrl(raw_url);
        data = raw_data;
    } 
    
    void Parser::run() {
        QObject eventProxy;
        QWebElement document;
        QWebElementCollection allLinkTags;
        QString currentRawUrl;
        QUrl currentUrl;

        qDebug() << "debug: in parse thread " << QThread::currentThreadId()
            << " beginning...";

        qDebug() << "debug: constructing web page instance";
        page = new QWebPage();

        qDebug() << "debug: setting html with "
            << data.size() << " bytes of data..";
        page->mainFrame()->setHtml(data, url);

        /*
         * suspend execution by locking a mutex, submitting 
         * a handle to this qthread to the threader, 
         * and waiting for qwebpage to emit signals
         */

        /* 
         * FIXME: wait for this to complete by connecting to a signal
         */

        qDebug() << "debug: retrieving document element...";
        document = page->mainFrame()->documentElement();

        qDebug() << "debug: finding all link tags";
        allLinkTags = document.findAll("a");

        foreach(QWebElement currentElement, allLinkTags) {
            currentRawUrl = currentElement.attribute("href");

            if (currentRawUrl != "") {
                owner->addUrl(QUrl(currentRawUrl));
            }
        }

        qDebug() << "debug: "
            << data.size() << " bytes in data string and "
            << page->totalBytes() << " bytes in page, and found "
            << allLinkTags.count() << " link tags";

        qDebug() << "debug: parse complete on thread "
            << QThread::currentThreadId();

        owner->dispatchRetrievers();
    }

    Parser::~Parser() {
        delete page;
    }

    void Parser::loadStartedCallback() {

    }
}                               /* namespace slurp */
