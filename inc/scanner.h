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

#ifndef SLURP_SCANNER_H
#define SLURP_SCANNER_H

#include <QRunnable>
#include <QWebPage>

#include "eventer.h"

namespace slurp {
    
    class Scanner : public QRunnable {
        
        QSharedPointer<QWebPage> page;

        Eventer* owner;

        public:

            Scanner( Eventer* owner, QSharedPointer<QWebPage> page );

            void run();

            inline Eventer* getOwner() {
                return owner;
            }

    };

}   /* namespace slurp */

#endif  /* SLUR_SCANNER_H */
