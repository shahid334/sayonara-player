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
void GUI_Player::playClicked(bool) {

    if(!m_metadata_available) {
        emit sig_play();
        return;
    }

    if (m_playing) {
        btn_play->setIcon(Helper::getIcon("play.png"));
		emit sig_pause();
	}

    else {

        btn_play->setIcon(Helper::getIcon("pause.png"));
		emit sig_play();
	}

	m_playing = !m_playing;
	m_trayIcon->setPlaying(m_playing);
}

void GUI_Player::stopClicked(bool b) {


    btn_play->setIcon(Helper::getIcon("play.png"));
    m_trayIcon->setPlaying(false);
	m_trayIcon->stop();
	m_playing = false;


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


	this->setWindowTitle("Sayonara");

	set_std_cover( false );

	if(b) {
		MetaData md;
		m_settings->setLastTrack(md);

        emit sig_stop();
    }


	if(btn_rec->isVisible() && btn_rec->isChecked()) {
		btn_rec->setChecked(false);
		emit sig_rec_button_toggled(false);
	}

}

void GUI_Player::backwardClicked(bool) {

   // albumCover->setFocus();
	int cur_pos_sec =  (m_completeLength_ms * songProgress->value()) / 100000;
	if(cur_pos_sec > 3) {
        setProgressJump(0);
    }

    else{
        emit sig_backward();
    }
}

void GUI_Player::forwardClicked(bool) {
	//albumCover->setFocus();
	emit sig_forward();
}


void GUI_Player::sl_rec_button_toggled(bool b) {

    emit sig_rec_button_toggled(b);
}



/** PROGRESS BAR **/
void GUI_Player::total_time_changed(qint64 total_time) {

	QString length_str = Helper::cvtMsecs2TitleLengthString(total_time, true);
	if(total_time == 0){
		length_str = "";
	}
    m_completeLength_ms = total_time;
	maxTime->setText(length_str);
	_md.length_ms = total_time;
	songProgress->setEnabled(total_time > 0);
}


void GUI_Player::jump_forward_ms(){
    emit sig_seek_rel_ms(10000);
}

void GUI_Player::jump_backward_ms(){
    emit sig_seek_rel_ms(-10000);
}



void GUI_Player::jump_forward() {

	int percent = songProgress->value();
    percent += 2;
    setProgressJump(percent);
	songProgress->setValue(percent);

}

void GUI_Player::jump_backward() {

	int percent = songProgress->value();
    percent -= 2;

    setProgressJump(percent);
	songProgress->setValue(percent);
}

void GUI_Player::setProgressJump(int percent) {

    if(percent > 100 || percent < 0) {
        percent = 0;
    }

    long cur_pos_ms = (percent * _md.length_ms) / 100;
    QString curPosString = Helper::cvtMsecs2TitleLengthString(cur_pos_ms);
    curTime->setText(curPosString);

	emit sig_seek_rel(percent);
}

void GUI_Player::psl_set_cur_pos(quint32 pos_sec) {

    if (m_completeLength_ms != 0) {

		int newSliderVal = (pos_sec * 100000) / (m_completeLength_ms);

		if (!songProgress->isSearching() && newSliderVal < songProgress->maximum()) {

			songProgress->setValue(newSliderVal);
		}
	}

	else if(pos_sec > m_completeLength_ms / 1000) {
		songProgress->setValue(0);
    }


	if(!songProgress->isSearching()) {

        if(m_completeLength_ms != 0 && pos_sec > m_completeLength_ms) pos_sec = 0;

        QString curPosString = Helper::cvtMsecs2TitleLengthString(pos_sec * 1000);
		curTime->setText(curPosString);
    }

}

/** PROGRESS BAR END **/

/** PLAYER BUTTONS END **/



/** VOLUME **/
void GUI_Player::setVolume(int vol) {
	volumeSlider->setValue(vol);
	setupVolButton(vol);
	emit sig_volume_changed(vol);
}

void GUI_Player::volumeChanged(int volume_percent) {
	setupVolButton(volume_percent);
	volumeSlider->setValue(volume_percent);
	emit sig_volume_changed(volume_percent);

    m_settings->setVolume(volume_percent);
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

    QString butFilename = Helper::getIconPath("vol_");

    if (percent <= 1) {
        butFilename += QString("mute") + m_skinSuffix + ".png";
	}

	else if (percent < 40) {
		butFilename += QString("1") + m_skinSuffix + ".png";
	}

	else if (percent < 80) {
		butFilename += QString("2") + m_skinSuffix + ".png";
	}

	else {
		butFilename += QString("3") + m_skinSuffix + ".png";
	}

	btn_mute->setIcon(QIcon(butFilename));

}

void GUI_Player::muteButtonPressed() {

	if (m_mute) {

		setupVolButton(volumeSlider->value());
		emit sig_volume_changed(volumeSlider->value());
	}

	else {

        setupVolButton(0);
		emit sig_volume_changed(0);
	}

    m_mute = !m_mute;

	volumeSlider->setDisabled(m_mute);
    m_trayIcon->setMute(m_mute);

	volumeSlider->update();
}

/** VOLUME END **/


