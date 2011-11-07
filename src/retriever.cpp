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

#include <iostream>

#include "retriever.h"

using namespace slurp;

Retriever::Retriever( const QString& uri, int flags ) {
  this -> uri = uri;
  conn = curl_easy_init();
}

Retriever::~Retriever() {
  if( conn ) {
     curl_easy_cleanup( conn );
  }
}

void Retriever::run() {
  /* perform the request */
}

bool Retriever::isValid() const {
  return (uri.length() > 0) && ( conn != NULL );
}
