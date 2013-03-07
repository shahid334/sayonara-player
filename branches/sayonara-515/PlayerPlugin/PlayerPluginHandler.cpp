#include "PlayerPluginHandler.h"
#include <QMap>
#include <QString>
#include <QAction>
#include <QDebug>

PlayerPluginHandler::PlayerPluginHandler(QObject *parent) :
    QObject(parent)
{
    _cur_shown_plugin = NULL;
}

PlayerPluginHandler::~PlayerPluginHandler(){}


PlayerPlugin* PlayerPluginHandler::find_plugin(QString name){

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
        emit sig_show_plugin(p);
    }

    else{
	
	hide_all();
        _cur_shown_plugin = NULL;
    }
}


void PlayerPluginHandler::showPlugin(PlayerPlugin* p){
     hide_all();

     p->show();
     p->getAction()->setChecked(true);
     _cur_shown_plugin = p;
}

// is called from player
void PlayerPluginHandler::showPlugin(QString name){

    PlayerPlugin* p = find_plugin(name);
    if(p){
        showPlugin(p);
    }
}


void PlayerPluginHandler::hide_all(){
   _cur_shown_plugin = NULL;
   foreach(PlayerPlugin* p, _plugins){
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

