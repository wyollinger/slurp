#include <QObject>
#include <QApplication>
#include <QPushButton>
#include <QUrl>
#include <iostream>
#include "parser.h"
#include "eventer.h"

using namespace parser;
int main(int argc, char* argv[]){
    Eventer ev(argc, argv);
    if(argc!=2){
        return 1;
    }
    QUrl url(argv[1]);
    Parser p(url); 
    p.requestPage();
    QObject::connect(&p, SIGNAL( finished(parseResult, Parser*) ), &ev, SLOT( writeOut(parseResult,  Parser*) ));
    QObject::connect(&p, SIGNAL( finished(parseResult, Parser*) ), &ev, SLOT( quit() ) ); 
    QObject::connect(&p, SIGNAL( parseFailed(QUrl, Parser*) ), &ev, SLOT( writeFail(QUrl, Parser*) ));
    QObject::connect(&p, SIGNAL( parseFailed(QUrl, Parser*) ), &ev, SLOT( quit() ) );
    std::cerr << "Past the connects\n";
    QPushButton hello("hi");
    if(argc ==2){
        hello.setText(argv[1]);
    }
    else{
        hello.setText("hello world");
    }
    hello.show();
    std::cerr << "Past the show, going into the event loop\n";

    return ev.exec();
}
