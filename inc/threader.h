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

#ifndef SLURP_THREADER_H
#define SLURP_THREADER_H

#include <QThreadPool>
#include <QWebFrame>

//#include "parser.h"
//#include "eventer.h"

namespace slurp {
    class Parser;
    class Eventer;
    class Threader : public QThreadPool {
        Q_OBJECT

        Eventer *owner; 

        public:

            Threader( Eventer* owner ) {
                this -> owner = owner;
            }

        public slots:

            void loadStartedCallback();
            void loadProgressCallback(int n);
            void loadFinishedCallback(bool ok);
            void frameCreationCallback(QWebFrame* frame);
            void contentsChangedCallback();
          
        private:

            
    };

}   /* namespace slurp */

#endif  /* SLURP_THREADER_H */
