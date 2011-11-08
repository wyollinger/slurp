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

#include "globals.h"
#include "retriever.h"
#include "eventer.h"
#include "scanner.h"

using namespace slurp;

Retriever::Retriever( const Eventer& eventer, const QString& uri, int flags ) {
  CURLMcode rc;
  this -> uri = uri;
  conn = curl_easy_init();

  /* TODO: get a handle to the creating retriever */

  /*
  curl_easy_setopt(conn, CURLOPT_URL, conn->url);
  curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, write_cb);
  curl_easy_setopt(conn, CURLOPT_WRITEDATA, &conn);
  curl_easy_setopt(conn, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, conn->error);
  curl_easy_setopt(conn, CURLOPT_PRIVATE, conn);
  curl_easy_setopt(conn, CURLOPT_NOPROGRESS, 0L);
  curl_easy_setopt(conn, CURLOPT_PROGRESSFUNCTION, prog_cb);
  curl_easy_setopt(conn, CURLOPT_PROGRESSDATA, conn);

  rc = curl_multi_add_handle(g->multi, conn->easy);
  mcode_or_die("new_conn: curl_multi_add_handle", rc);
 
  */
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
