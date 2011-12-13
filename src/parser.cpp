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
#include <QWebSettings>
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
        page = NULL;
    }

    void Parser::requestPage() {
        qDebug() << thread() << "parser: constructing page instance";

        page = new QWebPage();

        page->settings()->setAttribute( QWebSettings::AutoLoadImages, false );

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

        qDebug() << thread() << "parser: initiating page load";
        page->mainFrame()->load( url );
    }

    void Parser::cleanup() {
        /* TODO: fix stub */
    }   

    void Parser::parse() {
        QWebElementCollection linkTags = page->mainFrame()->findAllElements("a");

        if( linkTags.count() > 0 ) {
            foreach(QWebElement current, linkTags) {
                if ( current.attribute("href") != "") {
                    parsedUrls.push_back( QUrl( current.attribute("href")));
                }
            }
        }

        emit finished(parsedUrls);
    }

    void Parser::loadProgress(int n) {
        qDebug() << thread() << "parser: " << url << " load progress " << n ;
    }

    void Parser::frameLoadFinished(bool ok) {
        qDebug() << thread() << "parser: " << url << " frame load finished ok?: " << ok;
        emit parse();
    }

    void Parser::pageLoadFinished(bool ok) {
        qDebug() << thread() << "parser: " << url << " page load finished ok?: " << ok;
    }

    void Parser::frameCreated(QWebFrame *frame) {
        qDebug() << thread() << "parser: got new frame " << frame;
    }


}   /* namespace slurp */
