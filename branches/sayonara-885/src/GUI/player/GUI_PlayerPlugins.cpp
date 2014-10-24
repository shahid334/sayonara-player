/* GUI_PlayerPlugins.cpp */

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



#include "GUI/player/GUI_Player.h"


/** PLUGINS **/
QWidget* GUI_Player::getParentOfPlugin() {
	return plugin_widget;
}

void GUI_Player::setInfoDialog(GUI_InfoDialog* dialog) {
    ui_info_dialog = dialog;
	connect(ui_info_dialog, SIGNAL(sig_cover_changed(const CoverLocation&)),
			this, SLOT(sl_alternate_cover_available(const CoverLocation&)));
}


void GUI_Player::hideAllPlugins() {

    m_settings->setShownPlugin("");
	if(plugin_widget->isHidden()) return;

	int old_h = this->plugin_widget->minimumHeight();
    QSize pl_size = ui_playlist->size();

    pl_size.setHeight(pl_size.height() + old_h);
	plugin_widget->setMinimumHeight(0);

    ui_playlist->resize(pl_size);
}


void GUI_Player::showPlugin(PlayerPlugin* plugin) {

    hideAllPlugins();

    if(!plugin) return;
    if(!ui_playlist) return;

    QSize pl_size;
    QSize widget_size = plugin->getSize();

	int old_h = this->plugin_widget->minimumHeight();
	plugin_widget->show();

    _pph->showPlugin(plugin);


	pl_size = ui_playlist->size();

	widget_size.setWidth( plugin_widget->width() );
    pl_size.setHeight(pl_size.height() - widget_size.height() + old_h);

	plugin_widget->setMinimumHeight(plugin->getSize().height());
	_pph->resize(plugin_widget->size());

	ui_playlist->resize(pl_size);
    m_settings->setShownPlugin(plugin->getName());
}



/** PLUGINS **/
