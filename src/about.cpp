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

#include <QDialog>
#include <QLabel>
#include <QString>
#include <QVBoxLayout>

#include "globals.h"
#include "about.h"

namespace slurp {

    const QString About::aboutText = " slurp v"+
          QString('0'+VERSION_ID[0])+"."+
          QString('0'+VERSION_ID[1])+"."+
          QString('0'+VERSION_ID[2])+
          " coded by Max DeLiso <maxdeliso@gmail.com>";

    About::About(QWidget* parent) : 
        QDialog(parent, 
            Qt::Dialog | 
            Qt::WindowTitleHint | 
            Qt::WindowSystemMenuHint | 
            Qt::WindowCloseButtonHint | 
            Qt::CustomizeWindowHint ) 
                {
                    layout = new QVBoxLayout(this);
                    aboutLabel = new QLabel(aboutText, this);

                    layout->addWidget( aboutLabel );

                    setLayout(layout);
                }

}   /* namespace slurp */
