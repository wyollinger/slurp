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


#include <QDebug>
#include <QString>
#include <QThread>

#include "retriever.h" 
#include "eventer.h"
#include "callbacks.h"
#include "globals.h"
#include "util.h"

namespace slurp {

Retriever::Retriever( Eventer* eventer, QString uri, int flags ) {
  this -> uri = uri;
  this -> flags = flags;
 
  owner = eventer;
  socketEvent = NULL;
  errorBuffer[0] = '\0';
  dataBuffer = "";

  conn = curl_easy_init();
    
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
      curl_easy_setopt(
          conn, 
          CURLOPT_NOSIGNAL,
          1);

      owner -> addHandle( conn );
  } else {
      qDebug() << "error: could not initialize retriever curl handle";
  }

  qDebug() << "debug: retriever constructed";
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
           << " new handle@ " << curlHandle
           << " old handle@ " << conn 
	   << " new sockfd " << sockfd
	   << " old sockfd " << this -> sockfd
	   << " new action " << action 
	   << " old action " << this -> action;

  this -> sockfd = sockfd;
  this -> action = action;

  if( socketEvent ) { 
     event_del( socketEvent );
     socketEvent = NULL;
  }

  socketEvent = owner -> registerSocket( sockfd, kind );
  event_add( socketEvent, NULL );
}

size_t Retriever::bufferData( const char* data ) {
    dataBuffer += data;
    return dataBuffer.size();
}


} /* namespace slurp */

