#include "PlayerPluginHandler.h"
#include <QMap>
#include <QString>
#include <QAction>


PlayerPluginHandler::PlayerPluginHandler(QObject *parent) :
    QObject(parent)
{
    _cur_shown_plugin = NULL;
}

PlayerPluginHandler::~PlayerPluginHandler(){

}


PlayerPlugin* PlayerPluginHandler::_find_plugin(QString name){

	foreach(PlayerPlugin* p, _plugins){
		if(!p->getName().compare(name)) return p;
	}	

    return NULL;

}

void PlayerPluginHandler::addPlugin(PlayerPlugin* p){
    _plugins.push_back(p);
    connect(p, SIGNAL(sig_action_triggered(PlayerPlugin*,bool)), this, SLOT(plugin_action_triggered(PlayerPlugin*,bool)));
}


void PlayerPluginHandler::plugin_action_triggered(PlayerPlugin* p, bool b){

    if(b){
        if(_cur_shown_plugin) this->hide_all();

        _cur_shown_plugin = p;
        emit sig_show_plugin(p);
    }

    else{
        _cur_shown_plugin = NULL;
        emit sig_hide_all_plugins();
    }
}


QSize PlayerPluginHandler::getCurPluginSize(){
    if(_cur_shown_plugin) return _cur_shown_plugin->maximumSize();
    else return QSize(0,0);
}



void PlayerPluginHandler::showPlugin(PlayerPlugin* p){


     emit sig_show_plugin(p);

    _cur_shown_plugin = p;
     hide_all();
     p->show();

}

void PlayerPluginHandler::showPlugin(QString name){
    PlayerPlugin* p = _find_plugin(name);
    if(p){
        showPlugin(p);
    }
}

void PlayerPluginHandler::hide_all(){

    _cur_shown_plugin = NULL;

    foreach(PlayerPlugin* p, _plugins){
        p->hide();
	p->close();
    }

    emit sig_hide_all_plugins();
}



void PlayerPluginHandler::resize(QSize sz){

    if(!_cur_shown_plugin) return;

    _cur_shown_plugin->resize(sz);
}

QList<PlayerPlugin*> PlayerPluginHandler::get_all_plugins(){
	return _plugins;

}







