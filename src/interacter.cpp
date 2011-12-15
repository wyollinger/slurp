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

#include <QApplication>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QTextBrowser>
#include <QtGui/QWidget>
#include <QKeyEvent>

#include <QDebug>

#include "globals.h"
#include "interacter.h"

namespace slurp {

    Interacter::Interacter() {
        setFixedSize(512, 361);

        centralwidget = new QWidget(this);

        label = new QLabel("Slurp", centralwidget);
        label->setGeometry(QRect(20, 10, 471, 31));
        label->setTextFormat(Qt::AutoText);
        label->setAlignment(Qt::AlignCenter);

        progressBar = new QProgressBar(centralwidget);
        progressBar->setGeometry(QRect(20, 90, 481, 31));
        progressBar->setValue(0);

        queuedNumber = new QLCDNumber(centralwidget);
        queuedNumber->setGeometry(QRect(20, 190, 64, 23));

        crawledNumber = new QLCDNumber(centralwidget);
        crawledNumber->setGeometry(QRect(20, 150, 64, 23));

        urlBrowser = new QTextBrowser(centralwidget);
        urlBrowser->setGeometry(QRect(110, 150, 381, 181));

        splitter = new QSplitter(centralwidget);
        splitter->setGeometry(QRect(20, 50, 471, 25));
        splitter->setOrientation(Qt::Horizontal);

        urlEntry = new QLineEdit("http://", splitter);
        splitter->addWidget(urlEntry);

        crawlButton = new QPushButton("Crawl", splitter);
        splitter->addWidget(crawlButton);

        aboutButton = new QPushButton("About", splitter);
        splitter->addWidget(aboutButton);

        setCentralWidget(centralwidget);

        QObject::connect( crawlButton, SIGNAL(clicked()),
            this, SLOT(handleCrawlClicked()) );

        QObject::connect( aboutButton, SIGNAL(clicked()),
            this, SLOT(handleAboutClicked()) );

        QObject::connect( urlEntry, SIGNAL(textChanged(const QString&)), 
            this, SLOT(handleUrlChange(const QString&)) );
    }

    void Interacter::updateStats( int queued, int crawled ) {
        emit queuedNumber->display( queued );
        emit crawledNumber->display( crawled );
    }

    void Interacter::updateProgress( int n ) {
        emit progressBar->setValue( n );
    }

    void Interacter::handleCrawlClicked() {
        if( crawlButton -> text() == "Crawl" ) {
        
            QUrl seedUrl = QUrl( urlEntry->text() );

            if( seedUrl.isValid() && 
                seedUrl.host() != "" && 
                !seedUrl.isRelative() ) {

                emit crawlClicked( seedUrl );
                emit crawlStarted();

                crawlButton -> setText( "Stop" );

                qDebug() << "user clicked crawl with valid url. starting...";
            } else {
                /* TODO: warn user */
                qDebug() << "user clicked crawl but url was invalid. ignoring";
            }

       } else if( crawlButton -> text() == "Stop" ) {
           qDebug() << "in interface: user aborted crawl";

           emit crawlAborted();
           crawlButton -> setText( "Crawl" );
       }
    }

    void Interacter::handleAboutClicked() {

    }

    void Interacter::handleUrlChange(const QString& newUrl) {
        (void) newUrl;
    }

}   /* namespace slurp */
