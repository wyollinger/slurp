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

#include <QString>
#include <QQueue>
#include <QSet>

#include <cstdio>
#include <cerrno>

#include "eventer.h"
#include "retriever.h"

using namespace slurp;

Eventer::Eventer( const QQueue<QString>& initialURIs ) {
  pendingURIs = initialURIs;
}

int Eventer::run( int n ) {

  return 0;
}

void Eventer::queueURI( const QString& uri ) {

}


