/* Notification.h */

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



#ifndef NOTIFICATION_H_
#define NOTIFICATION_H_

#include "HelperStructs/Helper.h"

class Notification : public QObject {

Q_OBJECT

protected:

	bool	_initialized;
	QString _name;

public:

    virtual void notification_show(const MetaData&)=0;
    virtual void notification_update(const MetaData&)=0;
    virtual void not_close()=0;
	virtual QString get_name()=0;

};
	
Q_DECLARE_INTERFACE(Notification, "sayonara.notification/1.0")


#endif


