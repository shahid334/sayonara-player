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

    if (_plugins.keys().contains(name)) return _plugins[name];

    return NULL;

}

void PlayerPluginHandler::addPlugin(PlayerPlugin* p){
    _plugins[plugin->get_name()] = p;
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


QList<QAction*> PlayerPluginHandler::getPluginActions(){

    QList<QAction*> lst;
    foreach(QString key, _plugins.keys()){
        lst.push_back(_plugins[key]->getAction());
    }

}


void PlayerPluginHandler::showPlugin(PlayerPlugin* p){
    _cur_shown_plugin = p;
     hide_all();
     p->show();

     emit sig_show_plugin(p);
}

void PlayerPluginHandler::hide_all(){

    _cur_shown_plugin = NULL;

    foreach(QString key, _plugins.keys()){
        _plugins[key]->hide();
    }

    emit sig_hide_all_plugins();
}



void PlayerPluginHandler::resize(QSize sz){

    if(!_cur_shown_plugin) return;

    _cur_shown_plugin->resize(sz);
}







