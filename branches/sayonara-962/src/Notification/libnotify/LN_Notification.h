/* LN_Notification.h */

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




#ifndef LN_NOTIFICATION_H_
#define LN_NOTIFICATION_H_

#include "HelperStructs/SayonaraClass.h"
#include "Notification/Notification.h"
#include "HelperStructs/MetaData.h"


class LN_Notification : public Notification, protected SayonaraClass {

	Q_OBJECT
	Q_INTERFACES(Notification)

private:
    void* _not;

public:
	LN_Notification();
	virtual ~LN_Notification();

    virtual void notification_show(const MetaData& md);
    virtual void notification_update(const MetaData& md);
    virtual void not_close();
	virtual QString get_name();
};


#endif



