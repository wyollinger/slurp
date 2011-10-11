#ifndef RETRIEVER_H
#define RETRIEVER_H

#include <string>

class Retriever {
     std::string url;
     int socket;

     public:

     Retriever( const std::string& url );
     int createSocket();
     std::string readData();

     inline int getSocket() const {
         return socket;
     }
};

#endif 

