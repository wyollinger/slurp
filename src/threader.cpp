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
#include <QThread>
#include <QList>
#include <QObject>
#include <QWebElementCollection>

#include "threader.h"
#include "parser.h"
#include "scanner.h"

namespace slurp {

    void Threader::loadStartedCallback() {
        qDebug() << "debug: received load started callback from sender: " 
                 << QObject::sender();
    }

    void Threader::loadProgressCallback(int n) {
        QWebPage *currentPage = ((QWebPage*) QObject::sender());
        QWebFrame *mainFrame = currentPage -> mainFrame();
        QWebFrame *currentFrame = currentPage -> currentFrame();

        qDebug() << "debug: received load progress callback: " 
                 << n << " from sender: " << QObject::sender();

        qDebug() << "debug: in progress callback, main frame contains: "
                 << mainFrame -> findAllElements("a").count()
                 << " link tags and current frame contains "
                 << currentFrame -> findAllElements("a").count()
                 << " link tags";
    }

    void Threader::loadFinishedCallback(bool ok) {
        QWebFrame *currentFrame = ((QWebFrame*) QObject::sender());

        qDebug() << "debug: received load complete callback: " 
                 << ok << " from sender: " << currentFrame;

        if( ok ) {
            /* TODO: examine frame */
        }
    }

    void Threader::frameCreationCallback( QWebFrame* frame ) {
        qDebug() << "debug: received frame creation callback from page: "
                 << QObject::sender() << " on frame: " << frame;

        qDebug() << "debug: new frame running on thread "
                 << QObject::sender()->thread();

        QObject::connect( 
            frame,
            SIGNAL(loadFinished(bool)),
            this,
            SLOT(loadFrameFinishedCallback(bool)));
    }

    void Threader::contentsChangedCallback() {
        qDebug() << "debug: received contents changed callback from page: "
                 << QObject::sender();
    }

    void Threader::destroyedCallback() {
        qDebug() << "debug: received notification that QWebPage instance is being destroyed";
    }

    void Threader::loadFrameFinishedCallback(bool ok) {
        QWebFrame* currentFrame = (QWebFrame*) QObject::sender();

        qDebug() << "debug: received load frame finshed callback with status: " << ok
                 << "from frame :" << currentFrame << " with size " << currentFrame -> contentsSize();
    }

}   /* namespace slurp */
