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

#include "scanner.h"
#include "eventer.h"

namespace slurp {

    Scanner::Scanner( Eventer* owner, QWebPage* page ) {
        this->owner = owner;
        this->page = page;
    }

    void Scanner::run() {
        QWebElement document;
        QWebElementCollection allLinkTags;
        QString currentRawUrl;
        QUrl currentUrl;
           
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

        qDebug() << "debug: scan complete on thread "
            << QThread::currentThreadId();

        owner->dispatchRetrievers();
    }

}   /* namespace slurp */
