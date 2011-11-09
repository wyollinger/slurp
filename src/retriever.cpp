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

/* #include "globals.h" */
#include "retriever.h"
#include "eventer.h"
#include "scanner.h"

using namespace slurp;

Retriever::Retriever( Eventer* eventer, QString uri, int flags ) {
  this -> uri = uri;
  this -> flags = flags;
  owner = eventer;

  errorBuffer[0] = '\0';

  conn = curl_easy_init();
  if( conn ) {
    std::cout << "debug: constructed retriever with owner @" 
	      << owner << "\n";
  } else {
    die("error: could not create handle for retriever", EXIT_FAILURE);
  }
}

Retriever::~Retriever() {
  if( conn ) {
     curl_easy_cleanup( conn );
  }
}

void Retriever::run() {
    CURLMcode rc;

    if( conn ) {
      curl_easy_setopt(
          conn, 
          CURLOPT_URL, 
	  uri.data());
      curl_easy_setopt(
          conn, 
          CURLOPT_WRITEFUNCTION, 
	  Eventer::writeCallback);
      curl_easy_setopt(
          conn,
          CURLOPT_WRITEDATA,
	  this);
      curl_easy_setopt(
          conn, 
          CURLOPT_VERBOSE, 
	  flags & FLAGS_VERBOSE );
      curl_easy_setopt(
          conn, 
          CURLOPT_NOPROGRESS, 
	  !(flags & FLAGS_VERBOSE ));
      curl_easy_setopt( 
          conn, 
          CURLOPT_PROGRESSFUNCTION, 
          Eventer::progressCallback);
      curl_easy_setopt( 
          conn, 
          CURLOPT_PROGRESSDATA, 
          this);
      curl_easy_setopt(
          conn, 
          CURLOPT_ERRORBUFFER, 
	  errorBuffer);
      curl_easy_setopt( 
          conn, 
          CURLOPT_PRIVATE, 
          this);

      rc = curl_multi_add_handle(owner -> getMultiHandle(), conn);
      Eventer::curlVerify("curl_multi_add_handle from Retriever()", rc);

      std::cout << "debug: added retriever to multi handle owned by eventer @"
	        << owner << "\n";
  } else {
      std::cerr << "error: could not initialize retriever curl handle\n";
  }

}

bool Retriever::isValid() const {
  return (uri.length() > 0) && ( conn != NULL );
}
