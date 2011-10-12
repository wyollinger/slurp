#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "retriever.h"

using namespace slurp;

Retriever::Retriever( const std::string& url ) {
    this->url = url;
}

void Retriever::createSocket() {
    std::string hostname;
    addrinfo hints;   

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* TCP socket */
    hints.ai_protocol = 0;          /* Any protocol */

    

    /* getaddrinfo( hostname, 80,  */

    /* socket = ... */

    /* stub */
}

std::string Retriever::readData() {
    std::string data;

    /* stub */

    return data;
}

