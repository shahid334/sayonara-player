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
	return this->ui->plugin_widget;
}


void GUI_SimplePlayer::hideUnneededPlugins(QWidget* wannashow){
	if(ui_eq != wannashow)
		this->ui->action_ViewEqualizer->setChecked(false);

	if(ui_lfm_radio != wannashow)
		this->ui->action_ViewLFMRadio->setChecked(false);

	if(ui_stream != wannashow)
		this->ui->action_ViewStream->setChecked(false);

	if(ui_playlist_chooser != wannashow)
		this->ui->action_ViewPlaylistChooser->setChecked(false);
}

void GUI_SimplePlayer::hideAllPlugins(){

    if(ui_eq){
        this->ui_eq->hide();
        this->ui_eq->close();
    }

    if(ui_stream){
		this->ui_stream->hide();
        this->ui_stream->close();
    }

    if(ui_lfm_radio){
        this->ui_lfm_radio->hide();
        this->ui_lfm_radio->close();
    }

    if(ui_playlist_chooser){
        this->ui_playlist_chooser->hide();
        this->ui_playlist_chooser->close();
    }

	this->ui->plugin_widget->setMinimumHeight(0);
}

void GUI_SimplePlayer::check_show_plugins(){

	int shown_plugin = CSettingsStorage::getInstance()->getShownPlugin();

	switch(shown_plugin){

		case PLUGIN_EQUALIZER:
            if(!ui_eq) break;
			ui->action_ViewEqualizer->setChecked(true);
			show_eq(true);
			break;

		case PLUGIN_LFM_RADIO:
            if(!ui_lfm_radio) break;
			ui->action_ViewLFMRadio->setChecked(true);
			show_lfm_radio(true);
			break;

		case PLUGIN_STREAM:
            if(!ui_stream) break;
			ui->action_ViewStream->setChecked(true);
			show_stream(true);
			break;

		case PLUGIN_PLAYLIST_CHOOSER:
            if(!ui_playlist_chooser) break;
			ui->action_ViewPlaylistChooser->setChecked(true);
			show_playlist_chooser(true);
			break;



		case PLUGIN_NONE:
		default:
			break;
	}
}

void GUI_SimplePlayer::setEqualizer(GUI_Equalizer* eq) {
	ui_eq = eq;
    if(ui_eq)
        ui_eq->resize(this->ui->plugin_widget->size());
}

void GUI_SimplePlayer::setPlaylistChooser(GUI_PlaylistChooser* playlist_chooser){
	ui_playlist_chooser = playlist_chooser;
    if(ui_playlist_chooser)
        ui_playlist_chooser->resize(this->ui->plugin_widget->size());
}

void GUI_SimplePlayer::setStream(GUI_Stream* stream){
	ui_stream = stream;
    if(ui_stream)
        ui_stream->resize(this->ui->plugin_widget->size());
}

void GUI_SimplePlayer::setLFMRadio(GUI_LFMRadioWidget* radio){
	ui_lfm_radio = radio;
    if(ui_lfm_radio)
        ui_lfm_radio->resize(this->ui->plugin_widget->size());
}

void GUI_SimplePlayer::showPlugin(QWidget* widget, bool v){

	if(!widget) return;

	int old_h = this->ui->plugin_widget->minimumHeight();
    QSize widget_size = widget->size();

	QSize pl_size;
	if(ui_playlist)
		pl_size = ui_playlist->size();

	if (v){
		hideUnneededPlugins(widget);
		this->ui->plugin_widget->show();
		widget->show();
		widget_size.setWidth(this->ui->plugin_widget->width());
        pl_size.setHeight(pl_size.height() - widget_size.height() + old_h);

		this->ui->plugin_widget->setMinimumHeight(widget->height());
		widget->resize(widget_size);
	}

	else{
		widget->hide();
        widget->close();
        pl_size.setHeight(pl_size.height() + old_h);
		this->ui->plugin_widget->setMinimumHeight(0);

	}

	if(ui_playlist)
		ui_playlist->resize(pl_size);


}

void GUI_SimplePlayer::show_playlist_chooser(bool vis){

	if(vis) CSettingsStorage::getInstance()->setShownPlugin(PLUGIN_PLAYLIST_CHOOSER);
	else CSettingsStorage::getInstance()->setShownPlugin(PLUGIN_NONE);

	showPlugin(ui_playlist_chooser, vis);

}

void GUI_SimplePlayer::show_eq(bool vis) {

	if(vis) CSettingsStorage::getInstance()->setShownPlugin(PLUGIN_EQUALIZER);
	else CSettingsStorage::getInstance()->setShownPlugin(PLUGIN_NONE);

	showPlugin(ui_eq, vis);
}

void GUI_SimplePlayer::show_stream(bool vis){

	if(vis) CSettingsStorage::getInstance()->setShownPlugin(PLUGIN_STREAM);
	else CSettingsStorage::getInstance()->setShownPlugin(PLUGIN_NONE);

	showPlugin(ui_stream, vis);
}

void GUI_SimplePlayer::show_lfm_radio(bool vis){

	bool lfm_stuff = true;
	lfm_stuff &= CSettingsStorage::getInstance()->getLastFMActive();
	lfm_stuff &= LastFM::getInstance()->lfm_is_logged_in();

	if(vis && !lfm_stuff){
		QMessageBox::warning(this->ui->centralwidget, "LastFM warning", "Something is wrong with your LastFM settings");
	}

	vis &= lfm_stuff;


	if(vis) CSettingsStorage::getInstance()->setShownPlugin(PLUGIN_LFM_RADIO);
	else CSettingsStorage::getInstance()->setShownPlugin(PLUGIN_NONE);

	showPlugin(ui_lfm_radio, vis);
}

void GUI_SimplePlayer::close_playlist_chooser(){
	show_playlist_chooser(false);
	ui->action_ViewPlaylistChooser->setChecked(false);
}

void GUI_SimplePlayer::close_eq() {
	show_eq(false);
	ui->action_ViewEqualizer->setChecked(false);
}

void GUI_SimplePlayer::close_lfm_radio() {
	show_lfm_radio(false);
	ui->action_ViewLFMRadio->setChecked(false);
}

void GUI_SimplePlayer::close_stream() {
	show_stream(false);
	ui->action_ViewStream->setChecked(false);
}

/** PLUGINS **/
