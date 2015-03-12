/* GUI_Notifications.h */

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



#ifndef GUI_NOTIFICATIONS_H
#define GUI_NOTIFICATIONS_H

#include "HelperStructs/SayonaraClass.h"
#include "Notification/Notification.h"
#include "Notification/NotificationPluginLoader.h"
#include "GUI/ui_GUI_Notifications.h"

class GUI_Notifications : public SayonaraDialog, private Ui_GUI_Notification
{
    Q_OBJECT
public:
    explicit GUI_Notifications(QWidget *parent = 0);
    virtual ~GUI_Notifications();
    
private slots:
    void ok_clicked();

private:
    NotificationPluginLoader* _plugin_loader;

protected:
	virtual void language_changed();

};

#endif // GUI_NOTIFICATIONS_H
