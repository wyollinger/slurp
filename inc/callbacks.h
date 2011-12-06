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

#ifndef SLURP_CALLBACKS_H
#define SLURP_CALLBACKS_H

#include <curl/curl.h>

#include "globals.h"
#include "retriever.h"
#include "eventer.h"

namespace slurp {
     void eventCallback(int fd, short kind, void *userp);
     void timerCallback(int fd, short kind, void* oEventer);
     int multiTimerCallback( 
        CURLM * multi_handle, 
	long timeout_ms, 
	void* oEventer);
     void setSocket(
        Retriever* retriever, 
        curl_socket_t s, 
        CURL* e, 
        int act,  
        Eventer* eventer);
     void addSocket(
        curl_socket_t s, 
        CURL *easy, 
        int action, 
        Eventer* eventer);
     int socketCallback(
        CURL *e, 
	curl_socket_t s, 
	int what, 
	void *eventer, 
	void *retriever);
     size_t writeCallback(
        void *ptr, 
	size_t size, 
	size_t nmemb, 
	void *data);
     int progressCallback(
	void *p, 
	double dltotal, 
	double dlnow, 
	double ult,
        double uln);
     void keyboardCallback(
        evutil_socket_t s,
	short type, 
	void *data);

     void scanMultiInfo( Eventer* eventer );
}
#endif /* SLURP_CALLBACKS_H */
