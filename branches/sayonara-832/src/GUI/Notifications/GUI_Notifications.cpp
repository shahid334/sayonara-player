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

#include "HelperStructs/Style.h"
#include "HelperStructs/CSettingsStorage.h"

#include <QString>

GUI_Notifications::GUI_Notifications(QWidget *parent) :
    QDialog(parent)
{
    ui = new Ui_GUI_Notification();
    ui->setupUi(this);

    _plugin_loader = NotificationPluginLoader::getInstance();

    _settings = CSettingsStorage::getInstance();
    int timeout = _settings->getNotificationTimeout();
    bool active = _settings->getShowNotification();
    int scale = _settings->getNotificationScale();

    QList<Notification*> l_notifications = _plugin_loader->get_plugins();

    QString n_preferred_name = "Standard";
    Notification* n_preferred = _plugin_loader->get_cur_plugin();
    if(n_preferred)
        n_preferred_name = n_preferred->get_name();

    int idx = 0;
    for(int i=0; i<l_notifications.size(); i++){

        Notification* n = l_notifications[i];
        ui->combo_notification->addItem(n->get_name());
        if(!n_preferred_name.compare(n->get_name(), Qt::CaseInsensitive))
            idx = ui->combo_notification->count()-1;
    }

    ui->combo_notification->setCurrentIndex(idx);
    ui->sb_timeout->setValue(timeout);
    ui->cb_activate->setChecked(active);

    if(scale > 0)
	    ui->sb_scale->setValue(scale);
    else
	    ui->sb_scale->setValue(80);

    ui->sb_scale->setEnabled(scale > 0);
    ui->cb_scale->setChecked(scale > 0);

    connect(ui->btn_ok, SIGNAL(clicked()), this, SLOT(ok_clicked()));

}

GUI_Notifications::~GUI_Notifications(){

}


void GUI_Notifications::ok_clicked(){

    bool active =       ui->cb_activate->isChecked();
    int timeout =       ui->sb_timeout->value();
    QString cur_text =  ui->combo_notification->currentText();
    
    int scale = 	ui->sb_scale->value();
    if(!ui->cb_scale->isChecked()) scale = 0;

    _plugin_loader->set_cur_plugin(cur_text);

    _settings->setNotification(cur_text);
    _settings->setNotificationTimout(timeout);
    _settings->setShowNotifications(active);
    _settings->setNotificationScale(scale);

    emit sig_settings_changed(active, timeout);

    close();
}


void GUI_Notifications::change_skin(bool dark){

}

void GUI_Notifications::language_changed(){
    this->ui->retranslateUi(this);
}

