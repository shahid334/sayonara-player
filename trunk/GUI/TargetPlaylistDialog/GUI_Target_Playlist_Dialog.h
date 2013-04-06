/* GUI_Target_Playlist_Dialog.h */

/* Copyright (C) 2013  Lucio Carreras
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



#ifndef GUI_TARGET_PLAYLIST_DIALOG_H
#define GUI_TARGET_PLAYLIST_DIALOG_H

#include <QDialog>
#include "GUI/ui_GUI_Target_Playlist_Dialog.h"

class GUI_Target_Playlist_Dialog : public QDialog, private Ui_GUI_Target_Playlist_Dialog
{
    Q_OBJECT
public:
    GUI_Target_Playlist_Dialog(QWidget *parent = 0);
    virtual ~GUI_Target_Playlist_Dialog();
    void change_skin(bool dark);
    void language_changed();
    
signals:
    void sig_target_chosen(const QString&, bool);
    
private slots:
    void search_button_clicked();
    void ok_button_clicked();

private:
    Ui_GUI_Target_Playlist_Dialog* ui;
    
};

#endif // GUI_TARGET_PLAYLIST_DIALOG_H
