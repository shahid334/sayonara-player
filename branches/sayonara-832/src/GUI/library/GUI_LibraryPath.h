/* GUI_LibraryPath.h */

/* Copyright (C) 2011-2014  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



#ifndef GUI_LIBRARYPATH_H
#define GUI_LIBRARYPATH_H

#include <QWidget>
#include <QResizeEvent>
#include "GUI/ui_GUI_LibraryPath.h"


class GUI_LibraryPath : public QWidget, private Ui::GUI_SetLibrary
{
    Q_OBJECT
public:
    explicit GUI_LibraryPath(QWidget *parent = 0);
    
signals:
    void sig_library_path_set();
    
public slots:
    void language_changed();

private slots:
    void btn_clicked();

private:
    Ui::GUI_SetLibrary* ui;

protected:
    void resizeEvent(QResizeEvent* e);
    
};

#endif // GUI_LIBRARYPATH_H
