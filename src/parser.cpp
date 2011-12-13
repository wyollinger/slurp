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
#include "util.h"

namespace slurp {

    Parser::Parser(Eventer * owner, QUrl url) {
        this->owner = owner;
        this->url = url;
    }

    void Parser::run() {
        qDebug() << "parser: constructing new parser running with " << url 
                 << " in thread " << thread();

        page = new QWebPage();

        QObject::connect(
            page, SIGNAL(loadProgress(int)),
            this, SLOT(loadProgress(int)));
        
        QObject::connect(
            page->mainFrame(), SIGNAL(loadFinished(bool)),
            this, SLOT(frameLoadFinished(bool)));        

        QObject::connect(
            page, SIGNAL(loadFinished(bool)),
            this, SLOT(pageLoadFinished(bool)));

        QObject::connect(
            page, SIGNAL(frameCreated(QWebFrame*)),
            this, SLOT(frameCreated(QWebFrame*)));

        page->mainFrame()->load( url );
      
        exec();

        qDebug() << "parser: thread quitting";

        QWebElementCollection linkTags = page->mainFrame()->findAllElements("a");
        qDebug() << "parser: done with " << linkTags.count();

        if( linkTags.count() > 0 ) {
            foreach(QWebElement currentElement, linkTags) {
                    if ( currentElement.attribute("href") != "") {
                    owner -> addUrl(QUrl( currentElement.attribute("href")));
                }
            }
        }

        //delete page;
    }

    void Parser::loadProgress(int n) {
        qDebug() << "parser: " << url << " load progress " << n ;

        owner -> processEvents();
    }

    void Parser::frameLoadFinished(bool ok) {
        qDebug() << "parser: " << url << " frame load finished ok?: " << ok;
        quit();
    }

    void Parser::pageLoadFinished(bool ok) {
        qDebug() << "parser: " << url << " page load finished ok?: " << ok;
    }

    void Parser::frameCreated(QWebFrame *frame) {
        qDebug() << "parser: got new frame " << frame;
    }


}   /* namespace slurp */
