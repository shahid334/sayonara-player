/* GUI_Broadcast.h */

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



#ifndef GUI_BROADCAST_H
#define GUI_BROADCAST_H

#include "RadioStation/StreamServer.h"
#include "PlayerPlugin/PlayerPlugin.h"
#include "GUI/ui_GUI_Broadcast.h"


class GUI_Broadcast : public PlayerPlugin, private Ui::GUI_Broadcast
{
	Q_OBJECT

public:
	explicit GUI_Broadcast(QString name, StreamServer* server, QWidget *parent = 0);
	~GUI_Broadcast();

public slots:
	void new_connection_request(const QString& ip);
	void new_connection(const QString& ip);
	void connection_closed(const QString& ip);
	void can_listen(bool);

private slots:
	void dismiss_clicked();
	void dismiss_all_clicked();
	void option_clicked();
	void combo_changed(int idx);
	void retry();
	void mp3_enc_found();

private:
	void dismiss_at(int idx);
	void set_status_label();

protected:
	virtual void language_changed();

	StreamServer*		_server;
};

#endif // GUI_BROADCAST_H
