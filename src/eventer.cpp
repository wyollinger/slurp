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
#include "eventer.h"

using namespace slurp;

Eventer::Eventer( 
    const QQueue<QString>& initialURIs, 
    int quota, 
    int maxThreads ) {
      pendingURIs = initialURIs;
      this->quota = quota;

      threadPool.setMaxThreadCount( maxThreads );

      multi = curl_multi_init();
      curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, socketCallback);
      curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, timerCallback);
}

Eventer::~Eventer() {
  threadPool.waitForDone();	
  if( multi )
  {
    curl_multi_cleanup(multi);
  }
}

void Eventer::curlVerify(const char *where, CURLMcode code)
{
  if ( CURLM_OK != code ) {
    const char *s;
    switch (code) {
      case     CURLM_CALL_MULTI_PERFORM: s="CURLM_CALL_MULTI_PERFORM"; break;
      case     CURLM_BAD_HANDLE:         s="CURLM_BAD_HANDLE";         break;
      case     CURLM_BAD_EASY_HANDLE:    s="CURLM_BAD_EASY_HANDLE";    break;
      case     CURLM_OUT_OF_MEMORY:      s="CURLM_OUT_OF_MEMORY";      break;
      case     CURLM_INTERNAL_ERROR:     s="CURLM_INTERNAL_ERROR";     break;
      case     CURLM_UNKNOWN_OPTION:     s="CURLM_UNKNOWN_OPTION";     break;
      case     CURLM_LAST:               s="CURLM_LAST";               break;
      default: s="CURLM_unknown";
        break;
    case     CURLM_BAD_SOCKET:         s="CURLM_BAD_SOCKET";
       std::cerr << "ERROR: " << where << "returns" << s << std::endl;
      return;
    }
    std::cerr << "ERROR: " << where << "returns" << s << std::endl;
    exit(code);
  }
}

int Eventer::run() {

  //threadPool.start( new QRunnable() );
  //call libevent to start processing events
  return 0;
}

void Eventer::queueURI( const QString& uri ) {
  pendingURIs.enqueue( uri );
}

void Eventer::newConnection()
{
  CURLMcode rc;
 
  /*
  conn = calloc(1, sizeof(ConnInfo));
  memset(conn, 0, sizeof(ConnInfo));
  conn->error[0]='\0';
 
  conn->easy = curl_easy_init();
  if (!conn->easy) {
    fprintf(MSG_OUT, "curl_easy_init() failed, exiting!\n");
    exit(2);
  }
  conn->global = g;
  conn->url = strdup(url);
  curl_easy_setopt(conn->easy, CURLOPT_URL, conn->url);
  curl_easy_setopt(conn->easy, CURLOPT_WRITEFUNCTION, write_cb);
  curl_easy_setopt(conn->easy, CURLOPT_WRITEDATA, &conn);
  curl_easy_setopt(conn->easy, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(conn->easy, CURLOPT_ERRORBUFFER, conn->error);
  curl_easy_setopt(conn->easy, CURLOPT_PRIVATE, conn);
  curl_easy_setopt(conn->easy, CURLOPT_NOPROGRESS, 0L);
  curl_easy_setopt(conn->easy, CURLOPT_PROGRESSFUNCTION, prog_cb);
  curl_easy_setopt(conn->easy, CURLOPT_PROGRESSDATA, conn);
  fprintf(MSG_OUT,
          "Adding easy %p to multi %p (%s)\n", conn->easy, g->multi, url);
  rc = curl_multi_add_handle(g->multi, conn->easy);
  mcode_or_die("new_conn: curl_multi_add_handle", rc);
 
  */

  /* note that the add_handle() will set a time-out to trigger very soon so
     that the necessary socket_action() call will be called by this app */ 
}

void Eventer::eventCallback(int fd, short kind, void *userp)
{

}
 
void Eventer::timerCallback(int fd, short kind, void *userp) 
{

}

int Eventer::socketCallback(
        CURL *e, 
	curl_socket_t s, 
	int what, 
	void *cbp, 
	void *sockp)
{

}
 
size_t Eventer::writeCallback(
        void *ptr, 
	size_t size, 
	size_t nmemb, 
	void *data)
{

}
 
int Eventer::progressCallback(
	void *p, 
	double dltotal, 
	double dlnow, 
	double ult,
        double uln)
{

}

