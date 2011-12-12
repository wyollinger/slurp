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

    Scanner::Scanner( Eventer* owner, QWebFrame* frame ) {
        this->owner = owner;
        this->frame = frame;
    }

    void Scanner::run() {
        QWebElement document;
        QWebElementCollection allLinkTags;
        QString currentRawUrl;
        QUrl currentUrl;

        qDebug() << "debug: current frame contains "
                 << frame->contentsSize() 
                 << " bytes of information";

        qDebug() << "debug: retrieving document element";
        document = frame->documentElement();

        qDebug() << "debug: document element contains "
                 << frame->documentElement().findAll("a").count()
                 << " link tags";

        /*
        foreach(QWebElement currentElement, allLinkTags) {
            currentRawUrl = currentElement.attribute("href");

            if (currentRawUrl != "") {
                owner->addUrl(QUrl(currentRawUrl));
            }
        }*/

        qDebug() << "debug: scan complete";

        //owner->dispatchRetrievers();
    }

}   /* namespace slurp */
