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

#include "threader.h"
#include "parser.h"
#include "scanner.h"

namespace slurp {

    void Threader::loadStartedCallback() {
        qDebug() << "debug: received load started callback from sender: " 
                 << QObject::sender();
    }

    void Threader::loadProgressCallback(int n) {
        qDebug() << "debug: received load progress callback: " 
                 << n << " from sender: " << QObject::sender();

        if( n == 100 ) {
            qDebug() << "debug: stub to dispatch scanner thread in "
                     << QThread::currentThreadId();
        }
    }

    void Threader::loadFinishedCallback(bool ok) {
        qDebug() << "debug: received load complete callback: " 
                 << ok << " from sender: " << QObject::sender();
    }

    void Threader::frameCreationCallback( QWebFrame* frame ) {
        qDebug() << "debug: received frame creation callback from page: "
                 << QObject::sender() << " on frame: " << frame;
    }

    void Threader::contentsChangedCallback() {
        qDebug() << "debug: received contents changed callback from page: "
                 << QObject::sender();
    }

}   /* namespace slurp */
