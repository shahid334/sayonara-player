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
	_pph->hide_all();

	this->ui->plugin_widget->setMinimumHeight(0);
}


void GUI_SimplePlayer::showPlugin(PlayerPlugin* plugin){

	int old_h = this->ui->plugin_widget->minimumHeight();
        QSize widget_size = plugin->getSize();

	QSize pl_size;
	if(!ui_playlist) return;
	plugin->show();

	pl_size = ui_playlist->size();

	widget_size.setWidth( ui->plugin_widget->width() );
        pl_size.setHeight(pl_size.height() - widget_size.height() + old_h);

	ui->plugin_widget->setMinimumHeight(plugin->getSize().height());
	ui->plugin_widget->show();
       	
	_pph->resize(plugin_widget->size());

	ui_playlist->resize(pl_size);
}


/** PLUGINS **/
