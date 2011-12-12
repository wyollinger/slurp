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
#include <QWebPage>
#include <QWebElement>
#include <QWebFrame>
#include <QString>
#include <QUrl>
#include <QSharedPointer>

#include "scanner.h"
#include "eventer.h"

namespace slurp {

    Scanner::Scanner( Eventer* owner, QSharedPointer<QWebPage> page ) {
        this->owner = owner;
        this->page = page;
    }

    void Scanner::run() {
        QWebElement document;
        QWebElementCollection allLinkTags;
        QString currentRawUrl;
        QUrl currentUrl;
        
        qDebug() << "debug: in scanner with " 
                 << page->totalBytes() << " bytes of parsed html";   

        qDebug() << "debug: dumping main frame: "
                 << page->mainFrame()->renderTreeDump();
        qDebug() << "debug: main frame dump complete";

        qDebug() << "debug: dumping current frame: "
                << page->currentFrame()->renderTreeDump();
        qDebug() << "debug: current frame dump complete";

        qDebug() << "debug: retrieving document element";
        document = page->mainFrame()->documentElement();

        qDebug() << "debug: finding all link tags";
        allLinkTags = document.findAll("a");

        qDebug() << "debug: found " << allLinkTags.count() << " link tags";

        foreach(QWebElement currentElement, allLinkTags) {
            currentRawUrl = currentElement.attribute("href");

            if (currentRawUrl != "") {
                owner->addUrl(QUrl(currentRawUrl));
            }
        }

        qDebug() << "debug: scan complete on thread " << QThread::currentThreadId();

        owner->dispatchRetrievers();
        
    }

}   /* namespace slurp */
