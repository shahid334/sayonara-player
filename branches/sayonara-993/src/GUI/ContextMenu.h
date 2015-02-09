/* ContextMenu.h */

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



#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QMenu>
#include <QAction>
#include <QEvent>

#define ENTRY_INFO (1 << 0)
#define ENTRY_EDIT (1 << 1)
#define ENTRY_REMOVE (1 << 2)
#define ENTRY_DELETE (1 << 3)
#define ENTRY_PLAY_NEXT (1 << 4)
#define ENTRY_APPEND (1 << 5)


class ContextMenu : public QMenu
{
    Q_OBJECT

public:
    explicit ContextMenu(QWidget *parent = 0);
    virtual ~ContextMenu();
    void setup_entries(int entries);

    
signals:
    void sig_info_clicked();
    void sig_edit_clicked();
    void sig_remove_clicked();
    void sig_delete_clicked();
    void sig_play_next_clicked();
    void sig_append_clicked();

private slots:
    void info_clicked();
    void edit_clicked();
    void remove_clicked();
    void delete_clicked();
    void play_next_clicked();
    void append_clicked();

public slots:

private:
    QAction* 			_info_action;
    QAction* 			_edit_action;
    QAction* 			_remove_action;
    QAction* 			_delete_action;
    QAction*            _play_next_action;
    QAction*            _append_action;

    void clear_actions();

protected:
    void changeEvent(QEvent* e);
    
};

#endif // CONTEXTMENU_H
