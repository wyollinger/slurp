#ifndef RETRIEVER_H
#define RETRIEVER_H

#include <string>

namespace slurp {
  class Retriever {
     std::string url;
     int socket;   

     public:

     Retriever( const std::string& url );
     void createSocket();
     std::string readData();

     inline int getSocket() const {
         return socket;
     }

     inline std::string getUrl() const {
         return url;
     }

  };
}
#endif 

