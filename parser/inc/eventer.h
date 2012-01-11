#ifndef PARSER_EVENTER_H
#define PARSER_EVENTER_H

#include <QApplication>
#include <QTextStream>
#include "parser.h"
namespace parser {
    class Parser;
    class Eventer : public QApplication{
        Q_OBJECT

        public:
            Eventer(int& argc, char* argv[]);
            void writeOut(QList<QUrl>, Parser);
        private:
            QTextStream out;
            QTextStream in;
        signals:
            void startParser();
        public slots:
            void writeOut(parseResult, Parser*);
            void writeFail(QUrl, Parser*);
            void quit();
            
    };
}/*namespace parser*/
#endif /*PARSER_EVENTER_H*/
