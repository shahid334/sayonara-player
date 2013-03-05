/*
 * GUI_SimplePlayerPlugins.cpp
 *
 *  Created on: 10.10.2012
 *      Author: lugmair
 */

#include "GUI/player/GUI_Simpleplayer.h"

#include <QWidget>
#include <QAction>


/** PLUGINS **/


QWidget* GUI_SimplePlayer::getParentOfPlugin() {
	return ui->plugin_widget;
}

void GUI_SimplePlayer::setInfoDialog(GUI_InfoDialog* dialog){
    ui_info_dialog = dialog;
    ui_info_dialog->setMode(INFO_MODE_TRACKS);
}


void GUI_SimplePlayer::hideAllPlugins(){

    if(ui->plugin_widget->isHidden()) return;

    int old_h = this->ui->plugin_widget->minimumHeight();
    QSize pl_size = ui_playlist->size();

    pl_size.setHeight(pl_size.height() + old_h);

    ui->plugin_widget->setMinimumHeight(0);

    ui_playlist->resize(pl_size);
}


void GUI_SimplePlayer::showPlugin(PlayerPlugin* plugin){

    if(!plugin) return;
    if(!ui_playlist) return;


    QSize pl_size;
    QSize widget_size = plugin->getSize();

    int old_h = this->ui->plugin_widget->minimumHeight();
    ui->plugin_widget->show();

    _pph->showPlugin(plugin);


	pl_size = ui_playlist->size();

    widget_size.setWidth( ui->plugin_widget->width() );
    pl_size.setHeight(pl_size.height() - widget_size.height() + old_h);

    ui->plugin_widget->setMinimumHeight(plugin->getSize().height());
    _pph->resize(ui->plugin_widget->size());

	ui_playlist->resize(pl_size);
}


/** PLUGINS **/
