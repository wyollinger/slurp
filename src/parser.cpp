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

#include "globals.h"
#include "parser.h"
#include "eventer.h"
#include "util.h"

namespace slurp {

    Parser::Parser( QUrl url) {
        this->url = url;
        page = NULL;
    }
	
    void Parser::requestPage() {
        qDebug() << "parser: constructing page instance";

        page = new QWebPage(this);

        page->settings()->setAttribute( QWebSettings::JavascriptEnabled, false );
        page->settings()->setAttribute( QWebSettings::AutoLoadImages, false );
        page->settings()->setAttribute( QWebSettings::PluginsEnabled, false );

        QObject::connect(
            page, SIGNAL(loadProgress(int)),
            this, SLOT(loadProgress(int)));
        
        QObject::connect(
            page, SIGNAL(loadFinished(bool)),
            this, SLOT(pageLoadFinished(bool)));

        qDebug() << "parser: initiating page load";
        page->mainFrame()->load( url );
    }

    void Parser::cleanup() {
		/* TODO: stub */
    }   

    void Parser::parse() {
        QWebElementCollection linkTags = 
            page->mainFrame()->findAllElements("a");

        foreach(QWebElement current, linkTags) {
            if ( current.attribute("href") != "") {
                parsedUrls.push_back( QUrl( current.attribute("href")));
            }
        }

        emit finished(parsedUrls);
    }

    void Parser::loadProgress(int n) {
        qDebug() << "parser: " << url << " load progress " << n ;
		
		QCoreApplication::flush ();
    }

    void Parser::pageLoadFinished(bool ok) {
        if( ok ) {

            qDebug() << "parser: " 
                     << url 
                     << " page load finished ok.";

		    emit parse();
        } else { 
            qDebug() << "parser: failed to parse page " << url;
        }
    }

}   /* namespace slurp */
