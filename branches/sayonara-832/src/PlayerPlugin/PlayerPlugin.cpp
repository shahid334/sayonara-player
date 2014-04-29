/* PlayerPlugin.cpp */

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



#include "PlayerPlugin/PlayerPlugin.h"
#include <QDebug>

PlayerPlugin::PlayerPlugin(QString name, QString action_text, QWidget *parent) :
    QWidget(parent)
{
    _pp_name = name;
    _pp_is_shown = false;
    _pp_action = new QAction("", NULL);
    _pp_action->setCheckable(true);
    _pp_is_closed = true;
    connect(_pp_action, SIGNAL(triggered(bool)), this, SLOT(action_triggered(bool)));

}


PlayerPlugin::~PlayerPlugin(){
    if(_pp_action) delete _pp_action;
}

void PlayerPlugin::calc_action(QString action_text){

    int first_app_of_shortcut = action_text.indexOf("&") + 1;
    QString new_action_name = action_text;
    if(first_app_of_shortcut != -1 && first_app_of_shortcut < action_text.size()){

       _pp_action->setShortcut( QKeySequence(tr("Ctrl+") + new_action_name.at(first_app_of_shortcut) ) );
    }

    _pp_action->setText(new_action_name);
}

QString PlayerPlugin::getName(){
    return _pp_name;
}


QSize PlayerPlugin::getSize(){
    return this->minimumSize();
}


void PlayerPlugin::setSize(QSize size){
    this->setMinimumSize(size);
}


void PlayerPlugin::closeEvent(QCloseEvent* e){
    QWidget::close();

    _pp_action->setChecked(false);
    action_triggered(false);
}


void PlayerPlugin::action_triggered(bool b){

    _pp_action->setChecked(b);
    _pp_is_closed = !b;
    emit sig_action_triggered(this, b);
}

bool PlayerPlugin::isClosed(){
    return _pp_is_closed;
}

