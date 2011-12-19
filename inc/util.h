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

#ifndef SLURP_UTIL_H
#define SLURP_UTIL_H

#include <QQueue>
#include <QFile>

namespace slurp {
    
   extern QFile logFile; 

   int validateArgs(int argc, char **argv, char **env,
                QQueue < QString > &seedURIs, int &quota );
   void die(const char *errmsg, int errcode);
   void debugHandler(QtMsgType, const char* msg);

}   /* namespace slurp */

#endif  /* SLURP_UTIL_H */
