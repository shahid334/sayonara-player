/* PlaylistTabWidget.h */

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



#ifndef PLAYLISTTABBAR_H
#define PLAYLISTTABBAR_H

#include <QTabWidget>
#include <QTabBar>
#include <QLineEdit>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include "HelperStructs/Helper.h"
#include <QInputDialog>


class PlaylistTabMenu : public QMenu{

	Q_OBJECT

signals:
	void sig_delete_clicked();
	void sig_save_clicked();
	void sig_save_as_clicked();
	void sig_close_clicked();
	void sig_close_others_clicked();

private:
	QAction*	_action_delete;
	QAction*	_action_save;
	QAction*	_action_save_as;
	QAction*	_action_close;
	QAction*	_action_close_others;

public:

	PlaylistTabMenu(QWidget* parent=0);

	~PlaylistTabMenu();

	void show_menu_items(bool save_action, bool save_as_action, bool delete_action);
	void show_close(bool b);
};



class PlaylistTabBar : public QTabBar {

	Q_OBJECT

signals:
	void sig_tab_save(int tab_idx);
	void sig_tab_save_as(int tab_idx, const QString& name);
	void sig_tab_delete(int tab_idx);
	void sig_cur_idx_changed(int tab_idx);


private slots:
	void save_pressed();
	void save_as_pressed();
	void delete_pressed();
	void close_pressed();
	void close_others_pressed();

private:
	PlaylistTabMenu*	_menu;

	void mousePressEvent(QMouseEvent* e);

public:
	PlaylistTabBar(QWidget *parent=0);

	virtual ~PlaylistTabBar();

	void show_menu_items(bool save_action, bool save_as_action, bool delete_action);
	void setTabsClosable(bool b);
};



class PlaylistTabWidget : public QTabWidget
{
	Q_OBJECT

signals:
	void sig_tab_save(int tab_idx);
	void sig_tab_save_as(int tab_idx, const QString& name);
	void sig_tab_delete(int tab_idx);

private:
	PlaylistTabBar* _tab_bar;

public:
	PlaylistTabWidget(QWidget* parent=0);
	~PlaylistTabWidget();

	void show_tabbar();
	void hide_tabbar();

	void rename_tab(int idx, const QString& name);
	void show_menu_items(bool save_action, bool save_as_action, bool delete_action);

	void setTabsClosable(bool b);
};

#endif // PLAYLISTTABBAR_H
