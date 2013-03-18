#ifndef GUI_NOTIFICATIONS_H
#define GUI_NOTIFICATIONS_H

#include <QDialog>
#include "HelperStructs/CSettingsStorage.h"
#include "Notification/Notification.h"
#include "Notification/NotificationPluginLoader.h"
#include "GUI/ui_GUI_Notifications.h"

class GUI_Notifications : public QDialog, private Ui_GUI_Notification
{
    Q_OBJECT
public:
    explicit GUI_Notifications(QWidget *parent = 0);
    virtual ~GUI_Notifications();
    
signals:
    void sig_settings_changed(bool, int);


public slots:
    void change_skin(bool dark);

private slots:
    void ok_clicked();
private:
    Ui_GUI_Notification* ui;
    NotificationPluginLoader* _plugin_loader;

    CSettingsStorage* _settings;
};

#endif // GUI_NOTIFICATIONS_H
