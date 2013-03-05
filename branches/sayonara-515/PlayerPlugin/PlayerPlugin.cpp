#include "PlayerPlugin/PlayerPlugin.h"

PlayerPlugin::PlayerPlugin(QString name, QString action_text, QChar shortcut, QWidget *parent = 0) :
    QWidget(parent)
{
    _pp_name = name;
    _pp_is_shown = false;
    _pp_action = new QAction();
    _pp_action->setText(QString("&") + tr(action_text));

    int first_app_of_shortcut = action_text.indexOf(shortcut);
    QString new_action_name = action_text;
    if(first_app_of_shortcut != -1){
        new_action_name.insert(first_app_of_shortcut, "&");
       _pp_action->setShortcut(QKeySequence(QString("CTRL+") + shortcut));

    }

    _pp_action->setText(tr(new_action_name));

    connect(_pp_action, SIGNAL(triggered(bool)), this, SLOT(action_triggered(bool)));
}


virtual PlayerPlugin::~PlayerPlugin(){
    if(_pp_action) delete _pp_action;
}


QSize PlayerPlugin::getName(){
    return _pp_name;
}


QSize PlayerPlugin::getSize(){
    return _pp_size;
}


void PlayerPlugin::setSize(QSize size){
    _pp_size = size;
}


QAction* PlayerPlugin::getAction(){
    return _pp_action;
}


void PlayerPlugin::action_triggered(bool b){
    emit sig_action_triggered(this, b);
}

