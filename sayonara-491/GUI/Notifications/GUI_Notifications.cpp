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

    connect(ui->btn_ok, SIGNAL(clicked()), this, SLOT(ok_clicked()));
}

GUI_Notifications::~GUI_Notifications(){

}


void GUI_Notifications::ok_clicked(){

    bool active =       ui->cb_activate->isChecked();
    int timeout =       ui->sb_timeout->value();
    QString cur_text =  ui->combo_notification->currentText();

    _plugin_loader->set_cur_plugin(cur_text);

    _settings->setNotification(cur_text);
    _settings->setNotificationTimout(timeout);
    _settings->setShowNotifications(active);

    emit sig_settings_changed(active, timeout);

    close();
}


void GUI_Notifications::change_skin(bool dark){

}
