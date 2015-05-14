/* GUI_Notifications.cpp */

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



#include "GUI/Notifications/GUI_Notifications.h"
#include "Notification/Notification.h"
#include "Notification/NotificationPluginLoader.h"


GUI_Notifications::GUI_Notifications(QWidget *parent) :
	SayonaraDialog(parent),
	Ui_GUI_Notification()
{
	setupUi(this);

    _plugin_loader = NotificationPluginLoader::getInstance();

	int timeout = _settings->get(Set::Notification_Timeout);
	int active = _settings->get(Set::Notification_Show);
	int scale = _settings->get(Set::Notification_Scale);

    QList<Notification*> l_notifications = _plugin_loader->get_plugins();

    QString n_preferred_name = "Standard";
    Notification* n_preferred = _plugin_loader->get_cur_plugin();
    if(n_preferred)
        n_preferred_name = n_preferred->get_name();

    int idx = 0;
    for(int i=0; i<l_notifications.size(); i++) {

        Notification* n = l_notifications[i];
		combo_notification->addItem(n->get_name());
        if(!n_preferred_name.compare(n->get_name(), Qt::CaseInsensitive))
			idx = combo_notification->count()-1;
    }

	combo_notification->setCurrentIndex(idx);
	sb_timeout->setValue(timeout);
	cb_activate->setChecked(active);

    if(scale > 0)
		sb_scale->setValue(scale);
    else
		sb_scale->setValue(80);

	sb_scale->setEnabled(scale > 0);
	cb_scale->setChecked(scale > 0);

	connect(btn_ok, SIGNAL(clicked()), this, SLOT(ok_clicked()));
}

GUI_Notifications::~GUI_Notifications() {
}

void GUI_Notifications::language_changed(){
	retranslateUi(this);
}

void GUI_Notifications::ok_clicked() {

	bool active =       cb_activate->isChecked();
	int timeout =       sb_timeout->value();
	QString cur_text =  combo_notification->currentText();
    
	int scale = 	sb_scale->value();
	if(!cb_scale->isChecked()) scale = 0;

    _plugin_loader->set_cur_plugin(cur_text);

	_settings->set(Set::Notification_Name, cur_text);
	_settings->set(Set::Notification_Timeout, timeout);
	_settings->set(Set::Notification_Show, active);
	_settings->set(Set::Notification_Scale, scale);

    close();
}
