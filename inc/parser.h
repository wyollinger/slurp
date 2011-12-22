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

#ifndef SLURP_PARSER_H
#define SLURP_PARSER_H

#include <QList>
#include <QString>
#include <QObject>
#include <QThread>
#include <QWebPage>
#include <QUrl>

#include "globals.h"
#include "eventer.h"

namespace slurp {

    class Eventer;
    class Parser : public QObject {
        Q_OBJECT

        QUrl url;
        QWebPage *page;
        QList< QUrl > parsedUrls;

    public:

        Parser( QUrl url);
		static bool validateUrl( QUrl url );
	 
    public slots:
        
        void requestPage();
        void cleanup();

    signals:

        void finished( parseResult parsedUrls, Parser* parser );    
        void progress( int );
        void parseFailed( QUrl url );
        
    private slots:

        void parse();
        void loadProgress(int);
        void pageLoadFinished(bool);
    };

}   /* namespace slurp */

#endif  /* SLURP_PARSER_H */
