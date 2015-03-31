/* GUI_Podcasts.h */

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



#ifndef GUI_PODCASTS_H
#define GUI_PODCASTS_H


#include "GUI/ui_GUI_Podcasts.h"
#include "GUI/stream/AbstractStream.h"

#include <QMap>


class GUI_Podcasts : public AbstractStream, public Ui::GUI_Podcasts
{
    Q_OBJECT

protected:

	virtual void	language_changed();
	virtual bool	get_all_streams(QMap<QString, QString>& result);
	virtual bool	add_stream(QString station_name, QString url);
	virtual bool	delete_stream(QString station_name);

public:
	GUI_Podcasts(QString name, QWidget *parent = 0);
    virtual ~GUI_Podcasts();
};

#endif // GUI_PODCASTS_H
