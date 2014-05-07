/* PlayerPlugin.h */

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



#ifndef PLAYERPLUGIN_H
#define PLAYERPLUGIN_H

#include <QWidget>
#include <QAction>
#include <QCloseEvent>

class PlayerPlugin : public QWidget
{
    Q_OBJECT

public:
	PlayerPlugin(QString name, QWidget *parent = 0);
    virtual ~PlayerPlugin();

    
signals:

    void sig_action_triggered(PlayerPlugin*, bool);
    void sig_reload(PlayerPlugin*);

    
public slots:

private slots:
    void action_triggered(bool);

private:
    QString     _pp_name;
    bool        _pp_is_shown;
    bool        _pp_is_closed;


protected:
    QAction*    _pp_action;
    void setSize(QSize size);
    void closeEvent(QCloseEvent *);
	void calc_action();


public:

    QSize getSize();
    QString getName();

    //void setShown(bool b);
    bool isClosed();

	QAction* getAction();
	QString getVisName();


    
};

#endif // PLAYERPLUGIN_H
