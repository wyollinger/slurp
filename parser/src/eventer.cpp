#include<QApplication>
#include<QTextStream>
#include "eventer.h"

namespace parser {
    Eventer::Eventer(int& argc, char* argv []) : QApplication(argc, argv), out(stdout), in(stdin){
        //TODO: Change to socket implementation so I can have asynchronous read and writes
        //this->in = QTextStream(stdin);
        //this->out = QTextStream(stdout);

    }

    void Eventer::writeOut(parseResult list, Parser* p){
        for(int i =0; i<list.size(); ++i){
            this->out <<"URL: "<< list.at(i).toString() << "\n";
            this->out.flush();
        }

    }

    void Eventer::writeFail(QUrl url, Parser* p){
        this->out << "FAILED: " << url.toString() << "\n";
    }

    void Eventer::quit(){
        this->exit(0);

    }
    

}
