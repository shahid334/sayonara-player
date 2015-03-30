/* PlayerPluginHandler.h */

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
    void reload_plugin(PlayerPlugin*);

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
