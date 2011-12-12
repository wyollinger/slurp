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

#ifndef SLURP_PARSER_H
#define SLURP_PARSER_H

#include <QList>
#include <QString>
#include <QObject>
#include <QThread>
#include <QWebPage>
#include <QUrl>
#include <QMutex>

#include <csignal>


//#include "eventer.h"

namespace slurp {
    class Eventer;
    class Parser : public QThread {

        Eventer *owner;
        QUrl url;
        QSharedPointer<QWebPage> page;
        QString data; 
        struct sigaction action;

    public:

        Parser(Eventer * owner, QString raw_url, QString raw_data);

        void run();

        inline Eventer* getOwner() {
            return owner;
        }

        static void catchSegfault(int n);
    };

}   /* namespace slurp */

#endif  /* SLURP_PARSER_H */
