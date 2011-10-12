#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>

namespace slurp {
  class Tokenizer {
     std::string data;

     public:

     Tokenizer( const std::string& data );
     std::string next();
  };
}

#endif 
