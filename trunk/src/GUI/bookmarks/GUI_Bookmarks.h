/* GUI_Bookmarks.h */

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



#ifndef GUI_BOOKMARKS_H
#define GUI_BOOKMARKS_H


#include "DatabaseAccess/CDatabaseConnector.h"
#include "HelperStructs/MetaData/MetaData.h"
#include "PlayerPlugin/PlayerPlugin.h"
#include "GUI/ui_GUI_Bookmarks.h"
#include "PlayManager.h"
#include <QMap>

class GUI_Bookmarks : public PlayerPlugin, private Ui::GUI_Bookmarks
{
	Q_OBJECT
public:
	explicit GUI_Bookmarks(QString name, QWidget *parent = 0);
	

private slots:
	void combo_changed(int new_idx);
	void next_clicked();
	void prev_clicked();
	void new_clicked();
	void del_clicked();
	void del_all_clicked();
	void track_changed(const MetaData&);
	void pos_changed_ms(quint64 ms);


private:

	CDatabaseConnector*		_db;
	QMap<quint32, QString>	_bookmarks;
	PlayManager*			_play_manager;

	quint32 _cur_time;
	MetaData _md;

	void calc_prev(quint32 time_s);
	void calc_next(quint32 time_s);

	int _last_idx;
	int _next_idx;

	void enable_prev(int idx);
	void enable_next(int idx);
	void disable_prev();
	void disable_next();

protected:
	virtual void language_changed();

protected slots:
	virtual void playstate_changed(PlayManager::PlayState state);

};

#endif // GUI_BOOKMARKS_H
