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
#include <QPainter>
#include <QWidget>
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
        QWebElementCollection linkTags;
        QString currentRawUrl;


        qDebug() << "debug: in parse thread " << QThread::currentThreadId();

        qDebug() << "debug: constructing web page instance";
        page = QSharedPointer< QWebPage > ( new QWebPage() );

        qDebug() << "debug: page instance running in thread: " << page->thread();

        qDebug() << "debug: beginning parse of page: " << url;
        page->mainFrame()->setHtml(data, url);

        qDebug() << "debug: post setHtml/load calls";
        qDebug() << "debug: dumping page children and thread info";

        foreach( QObject* cchild, page->children() ) {
            qDebug() << "debug: page child " << cchild << " on thread " << cchild -> thread();
        }

        qDebug() << "debug: dumping main frame children";

        foreach( QObject* cchild, page->mainFrame()->children() ) {
            qDebug() << "debug: dumping frame " << cchild;    
        }

        linkTags = page->mainFrame()->findAllElements("a");

        qDebug() << "debug: after parse, main frame contains " 
                 << linkTags.count() << " links";
       
        if( linkTags.count() > 0 ) {
            foreach(QWebElement currentElement, linkTags) {
                currentRawUrl = currentElement.attribute("href");

                    if (currentRawUrl != "") {
                    owner->addUrl(QUrl(currentRawUrl));
                }
            }
        }

       // page.clear(); 

        owner -> dispatchRetrievers();
    }
}   /* namespace slurp */
