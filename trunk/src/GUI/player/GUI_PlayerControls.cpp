// GUI_PlayerButtons.cpp

/* Copyright (C) 2011 - 2014  Lucio Carreras
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
#include "GUI/player/GUI_TrayIcon.h"
#include <QFileDialog>

/** PLAYER BUTTONS **/
void GUI_Player::playstate_changed(PlayManager::PlayState state){
	switch(state){
		case PlayManager::PlayState_Playing:
			played();
			break;
		case PlayManager::PlayState_Paused:
			paused();
			break;
		case PlayManager::PlayState_Stopped:
			stopped();
			break;

		default:
			return;
	}

	return;
}


void GUI_Player::play_clicked(){
	m_play_manager->play_pause();
}

void GUI_Player::played() {
	m_trayIcon->setPlaying(true);
	btn_play->setIcon(Helper::getIcon("pause"));
}

void GUI_Player::paused() {
	m_trayIcon->setPlaying(false);
	btn_play->setIcon(Helper::getIcon("play"));
}


void GUI_Player::stop_clicked(){
	m_play_manager->stop();
}


void GUI_Player::stopped() {

	setWindowTitle("Sayonara");

	btn_play->setIcon(Helper::getIcon("play"));
    m_trayIcon->setPlaying(false);
	m_trayIcon->stop();

	lab_title->hide();
	lab_sayonara->show();

	lab_artist->hide();
	lab_writtenby->show();

	lab_album->hide();
	lab_version->show();

	lab_rating->hide();
	lab_copyright->show();

	songProgress->setValue(0);
	songProgress->setEnabled(false);

	curTime->setText("00:00");
	maxTime->setText("00:00");

	m_metadata_available = false;
	set_std_cover( false );

	if(btn_rec->isVisible() && btn_rec->isChecked()) {
		btn_rec->setChecked(false);
		emit sig_rec_button_toggled(false);
	}
}

void GUI_Player::prev_clicked() {
	m_play_manager->previous();
}

void GUI_Player::next_clicked() {
	m_play_manager->next();
}


void GUI_Player::sl_rec_button_toggled(bool b) {

    emit sig_rec_button_toggled(b);
}



/** PROGRESS BAR **/

void GUI_Player::set_cur_pos_label(int val){

	int max = songProgress->maximum();
	if(val > max || val < 0) {
		val = 0;
	}

	double percent = (val * 1.0) / max;
	quint64 cur_pos_ms =  (quint64) (percent * _md.length_ms);

	QString curPosString = Helper::cvt_ms_to_string(cur_pos_ms);

	curTime->setText(curPosString);
}

void GUI_Player::total_time_changed(qint64 total_time) {

	QString length_str = Helper::cvt_ms_to_string(total_time, true);
	if(total_time == 0){
		length_str = "";
	}

	maxTime->setText(length_str);
	_md.length_ms = total_time;
	songProgress->setEnabled(total_time > 0);
}


void GUI_Player::jump_forward_ms(){
	//emit sig_seek_rel_ms(10000);
}

void GUI_Player::jump_backward_ms(){
	//emit sig_seek_rel_ms(-10000);
}



void GUI_Player::jump_forward() {
	songProgress->increment(50);
}

void GUI_Player::jump_backward() {
	songProgress->increment(-50);
}

void GUI_Player::seek(int val) {

	if(val < 0) return;
	set_cur_pos_label(val);

	double percent = (val * 1.0) / songProgress->maximum();
	m_play_manager->seek_rel(percent);
}

void GUI_Player::psl_set_cur_pos_ms(quint64 pos_ms) {

	int max = songProgress->maximum();
	int new_val;

	if ( _md.length_ms > 0 ) {
		new_val = ( pos_ms * max ) / (_md.length_ms);
	}

	else if(pos_ms > _md.length_ms) {
		new_val = 0;
    }

	else{
		return;
	}

	songProgress->setValue(new_val);

	QString curPosString = Helper::cvt_ms_to_string(pos_ms);
	curTime->setText(curPosString);

}

/** PROGRESS BAR END **/

/** PLAYER BUTTONS END **/



/** VOLUME **/

void GUI_Player::volumeChanged(int volume_percent) {
	setupVolButton(volume_percent);
	volumeSlider->setValue(volume_percent);

	/// TODO: endless recursion??
	_settings->set(Set::Engine_Vol, volume_percent);
}

void GUI_Player::volumeChangedByTick(int val) {


	int currentVolumeOrig_perc = this -> volumeSlider->value();
    int currentVolume_perc = currentVolumeOrig_perc;
    int vol_step = m_trayIcon->get_vol_step();

    if (val > 0) {
        //increase volume
		if (currentVolume_perc < volumeSlider->maximum() - vol_step) {
            currentVolume_perc += vol_step;
        }

        else currentVolumeOrig_perc = 100;
    }

    else if (val < 0) {
        //decrease volume
		if (currentVolume_perc > volumeSlider->minimum() + vol_step) {
            currentVolume_perc -= vol_step;
        }

        else currentVolume_perc = 0;
    }



    if (currentVolumeOrig_perc != currentVolume_perc) {

        volumeChanged(currentVolume_perc);
    }
}


void GUI_Player::volumeHigher() {
    volumeChangedByTick(5);
}

void GUI_Player::volumeLower() {
    volumeChangedByTick(-5);
}

void GUI_Player::setupVolButton(int percent) {

	QString butFilename = "vol_";

    if (percent <= 1) {
		butFilename += QString("mute") + m_skinSuffix;
	}

	else if (percent < 40) {
		butFilename += QString("1") + m_skinSuffix;
	}

	else if (percent < 80) {
		butFilename += QString("2") + m_skinSuffix;
	}

	else {
		butFilename += QString("3") + m_skinSuffix;
	}

	btn_mute->setIcon( Helper::getIcon(butFilename) );

}

void GUI_Player::muteButtonPressed() {

	m_mute = !m_mute;

	int vol = 0;
	if (!m_mute) {
		vol = volumeSlider->value();
	}

	volumeSlider->setDisabled(m_mute);
    m_trayIcon->setMute(m_mute);
	setupVolButton(vol);

	_settings->set(Set::Engine_Vol, vol);
}

/** VOLUME END **/


