#ifndef RETRIEVER_H
#define RETRIEVER_H

#include <string>

class Retriever {
     std::string url;

     public:
     Retriever( const std::string& url );
     std::string getData();
};

#endif 

