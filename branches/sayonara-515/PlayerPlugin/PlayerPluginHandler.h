#ifndef PLAYERPLUGINHANDLER_H
#define PLAYERPLUGINHANDLER_H

#include <QObject>
#include <QList>
#include <QString>
#include "PlayerPlugin/PlayerPlugin.h"




class PlayerPluginHandler : public QObject
{
    Q_OBJECT
public:
    PlayerPluginHandler(QObject *parent = 0);
    ~PlayerPluginHandler();
    
signals:
    void sig_show_plugin(PlayerPlugin*);
    void sig_hide_all_plugins();
    
public slots:
    void resize(QSize sz);
    void hide_all();

private slots:
    void plugin_action_triggered(PlayerPlugin*, bool);

private:
    QList<PlayerPlugin*>  _plugins;
    PlayerPlugin*       _cur_shown_plugin;





public:
    void addPlugin(PlayerPlugin*);
    void showPlugin(PlayerPlugin*);
    void showPlugin(QString name);

    PlayerPlugin*        find_plugin(QString name);
    QList<PlayerPlugin*> get_all_plugins();
};

#endif // PLAYERPLUGINHANDLER_H
