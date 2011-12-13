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


#ifndef SLURP_INTERACTER_H
#define SLURP_INTERACTER_H

#include <QMainWindow>
#include <QLabel>

namespace slurp {

    class Interacter : public QMainWindow {
        Q_OBJECT

        QLabel* mainLabel;

        public:

            Interacter();
    };

}    /* namespace slurp */


#endif    /* SLURP_INTERACTER_H */
