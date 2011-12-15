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


#ifndef SLURP_INTERACTER_H
#define SLURP_INTERACTER_H

#include <QMainWindow>
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

namespace slurp {

    class Interacter : public QMainWindow {
        Q_OBJECT

        QWidget *centralwidget;
        QLabel *label;
        QProgressBar *progressBar;
        QLCDNumber *queuedNumber;
        QLCDNumber *crawledNumber;
        QTextBrowser *urlBrowser;
        QSplitter *splitter;
        QLineEdit *urlEntry;
        QPushButton *crawlButton;
        QPushButton *aboutButton;

        public:

            Interacter();
    };

}    /* namespace slurp */



#endif    /* SLURP_INTERACTER_H */
