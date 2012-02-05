/*
 * LyricMenuButton.cpp
 *
 *  Created on: Feb 5, 2012
 *      Author: luke
 */

#include "GUI/LyricMenuButton/LyricMenuButton.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Style.h"

#include <QIcon>
#include <QList>
#include <QMenu>
#include <QAction>
#include <QToolButton>
#include <QStringList>
#include <QDebug>

LyricMenuButton::LyricMenuButton(QWidget* parent) : QToolButton(parent) {

	_menu = new QMenu(0);
	this->setText("");


	connect(_menu, SIGNAL(triggered(QAction*)), this, SLOT(server_changed(QAction*)));

}

LyricMenuButton::~LyricMenuButton() {
	// TODO Auto-generated destructor stub
}


void LyricMenuButton::server_changed(QAction* action){

	foreach(QAction* a, _actions){
		if(a->data() != action->data()){
			a->setChecked(false);
		}
		else a->setChecked(true);
	}



	//this->setText(_servers[action->data().toInt()]);
	emit sig_server_changed(action->data().toInt());
}

void LyricMenuButton::setServers(const QStringList& servers){

	if(servers.size() == 0) return;

	for(int i=0; i<servers.size(); i++){
		QString str = servers[i];
		QAction* action = new QAction(str, 0);
		action->setData(i);
		action->setCheckable(true);
		if(i==0) action->setChecked(true);
		_actions.push_back(action);
		_menu->addAction(action);
	}

	this->setDefaultAction(_actions[0]);
	this->setMenu(_menu);
	_servers = servers;
}

