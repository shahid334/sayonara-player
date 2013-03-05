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
    explicit PlayerPluginHandler(QObject *parent = 0);
    
signals:
    void sig_show_plugin(Plugin*);
    void sig_hide_all_plugins();
    
public slots:
    void resize(QSize sz);
    void hide_all();

private slots:
    void plugin_action_triggered(PlayerPlugin*, bool);

private:
    QMap<PlayerPlugin>  _plugins;
    PlayerPlugin*       _cur_shown_plugin;

    PlayerPlugin*       _find_plugin(QString name);



public:
    void addPlugin(PlayerPlugin*);
    void showPlugin(QString name, bool b=true);

    QSize getCurPluginSize();


    
};

#endif // PLAYERPLUGINHANDLER_H
