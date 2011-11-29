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

/*
 * note: the example hiperfifo.c by Jeff Pohlmeyer was referenced extensively
 * while implementing this class.
 */

#include <iostream>

#include "globals.h" 

using namespace slurp;

Retriever::Retriever( Eventer* eventer, QString uri, int flags ) {
  this -> uri = uri;
  this -> flags = flags;
 
  owner = eventer;
  socketEvent = NULL;
  errorBuffer[0] = '\0';

  setAutoDelete(false);

  conn = curl_easy_init();

  qDebug() << "debug: constructed retriever with owner @" 
	      << owner;
}

Retriever::~Retriever() {
  qDebug() << "debug: deleting retriever instance";
  
  if( conn ) {
     curl_easy_cleanup( conn );
  }

  if( socketEvent ) {
     event_del( socketEvent ); 
  }
}

void Retriever::setSocketData( curl_socket_t sockfd, int action, int kind, CURL* curlHandle ) {
  qDebug() << "debug: setting socket data with sock " << sockfd 
           << " action " << action
           << " kind " << kind 
           << " and handle@ " << curlHandle;
  qDebug() << "debug: the handle of this retriever is at " << conn;

  /* curlHandle should already be stored in the retriever */
  
  this -> sockfd = sockfd;
  this -> action = action;

  if( socketEvent ) { 
     event_del( socketEvent );
     /* delete socketEvent;  this may  be needed */
     socketEvent = NULL;
  }

  /* todo: add error checking */
  socketEvent = event_new( owner -> getEventBase(), sockfd, kind, eventCallback, owner);
  event_add( socketEvent, NULL );
}


void Retriever::run() {
    CURLMcode rc;

    if( conn ) {  
      curl_easy_setopt(
          conn, 
          CURLOPT_URL, 
	  uri.toAscii().data());
      curl_easy_setopt(
          conn, 
          CURLOPT_WRITEFUNCTION, 
	  writeCallback);
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
	  !(flags & FLAGS_VERBOSE) );
      curl_easy_setopt( 
          conn, 
          CURLOPT_PROGRESSFUNCTION, 
          progressCallback);
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
      curlVerify("curl_multi_add_handle from Retriever()", rc);

      qDebug() << "debug: added retriever with easy @"
	        << conn << " to multi handle owned by eventer @"
	        << owner << " with multi @" << owner->getMultiHandle() 
		<< "and target of: " << uri.toAscii().data() 
                << " with address @" << this;
  } else {
      qDebug() << "error: could not initialize retriever curl handle";
  }

}

bool Retriever::isValid() const {
  return (uri.length() > 0) && ( conn != NULL );
}
