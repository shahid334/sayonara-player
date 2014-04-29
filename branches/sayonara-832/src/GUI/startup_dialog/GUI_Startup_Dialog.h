/* GUI_Startup_Dialog.h */

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



#ifndef GUI_STARTUP_DIALOG_H
#define GUI_STARTUP_DIALOG_H

#include <QDialog>
#include "GUI/ui_GUI_Startup_Dialog.h"

class GUI_Startup_Dialog : public QDialog, private Ui::GUI_StartupDialog
{
    Q_OBJECT
public:
    GUI_Startup_Dialog(QWidget *parent = 0);
    virtual ~GUI_Startup_Dialog();
    
signals:
    
public slots:
    void language_changed();

private slots:
    void cb_toggled(bool);
    void ok_clicked();

private:
    Ui::GUI_StartupDialog* ui;
    
};

#endif // GUI_STARTUP_DIALOG_H
