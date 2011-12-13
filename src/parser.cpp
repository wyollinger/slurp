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
        qDebug() << "parser: new parser running with " << url 
                 << " in thread " << thread();

        qDebug() << "parser: constructing page instance";
        page = new QWebPage();

        qDebug() << "parser: starting load";
        page->mainFrame()->load( url );

        qDebug() << "parser: done";
    }


}   /* namespace slurp */
