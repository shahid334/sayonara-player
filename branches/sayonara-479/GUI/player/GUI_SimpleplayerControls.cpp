// GUI_SimpleplayerButtons.cpp

/* Copyright (C) 2012  Lucio Carreras
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

#include "HelperStructs/Helper.h"
#include "HelperStructs/MetaData.h"
#include "GUI/player/GUI_Simpleplayer.h"
#include "GUI/player/GUI_TrayIcon.h"


#include <QString>
#include <QIcon>

/** PLAYER BUTTONS **/
void GUI_SimplePlayer::playClicked(bool) {

    if(!m_metadata_available) return;

    if (m_playing) {
		ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
		emit pause();
	}

    else {
		ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "pause.png"));
		emit play();
	}

	m_playing = !m_playing;
        m_trayIcon->switch_play_pause(m_playing);
}

void GUI_SimplePlayer::stopClicked(bool) {

	ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
	m_trayIcon->switch_play_pause(false);

	m_playing = false;

	ui->title->setText("Sayonara Player");
	ui->rating->setText("");
	ui->album->setText("Written by Lucio Carreras");
    ui->artist->setText(CSettingsStorage::getInstance()->getVersion());
	this->setWindowTitle("Sayonara");
	ui->songProgress->setValue(0);
    ui->curTime->setText("0:00");
	ui->maxTime->setText("0:00");

    ui->albumCover->setIcon(QIcon(Helper::getIconPath() + "logo.png"));

	if(ui->btn_rec->isVisible() && ui->btn_rec->isChecked()){
		ui->btn_rec->setChecked(false);
        emit sig_rec_button_toggled(false);
	}

	emit stop();
}

void GUI_SimplePlayer::backwardClicked(bool) {

   // ui->albumCover->setFocus();
    int cur_pos_sec =  (m_completeLength_ms * ui->songProgress->value()) / 100000;
    if(cur_pos_sec > 3){
        setProgressJump(0);
    }

    else{
        emit backward();
    }
}

void GUI_SimplePlayer::forwardClicked(bool) {
    //ui->albumCover->setFocus();
	emit forward();
}


void GUI_SimplePlayer::sl_rec_button_toggled(bool b){

    emit sig_rec_button_toggled(b);
}



/** PROGRESS BAR **/
void GUI_SimplePlayer::total_time_changed(qint64 total_time) {

	QString length_str = Helper::cvtMsecs2TitleLengthString(total_time, true);
    m_completeLength_ms = total_time;
    ui->maxTime->setText(length_str);
}

void GUI_SimplePlayer::setProgressJump(int percent){

    if(percent > 100 || percent < 0) {
        percent = 0;
    }
    long cur_pos_ms = (percent * m_metadata.length_ms) / 100;
    QString curPosString = Helper::cvtMsecs2TitleLengthString(cur_pos_ms);
    ui->curTime->setText(curPosString);

    emit search(percent);
}

void GUI_SimplePlayer::setCurrentPosition(quint32 pos_sec) {

    if (m_completeLength_ms != 0) {

		int newSliderVal = (pos_sec * 100000) / (m_completeLength_ms);

        if (!ui->songProgress->isSearching() && newSliderVal < ui->songProgress->maximum()){

            ui->songProgress->setValue(newSliderVal);
		}
	}

    else if(pos_sec > m_completeLength_ms / 1000){
    	ui->songProgress->setValue(0);
    }


    if(!ui->songProgress->isSearching()){

        if(m_completeLength_ms != 0 && pos_sec > m_completeLength_ms) pos_sec = 0;

        QString curPosString = Helper::cvtMsecs2TitleLengthString(pos_sec * 1000);
        ui->curTime->setText(curPosString);
    }

}

/** PROGRESS BAR END **/

/** PLAYER BUTTONS END **/



/** VOLUME **/
void GUI_SimplePlayer::setVolume(int vol) {
	ui->volumeSlider->setValue(vol);
	setupVolButton(vol);
	emit sig_volume_changed(vol);
}

void GUI_SimplePlayer::volumeChanged(int volume_percent) {
	setupVolButton(volume_percent);
    ui->volumeSlider->setValue(volume_percent);
	emit sig_volume_changed(volume_percent);

	CSettingsStorage::getInstance()->setVolume(volume_percent);
}

void GUI_SimplePlayer::volumeChangedByTick(int val) {


    int currentVolumeOrig_perc = this -> ui->volumeSlider->value();
    int currentVolume_perc = currentVolumeOrig_perc;
    int vol_step = m_trayIcon->get_vol_step();

    if (val > 0) {
        //increase volume
        if (currentVolume_perc < ui->volumeSlider->maximum() - vol_step) {
            currentVolume_perc += vol_step;
        }
    }

    else if (val < 0) {
        //decrease volume
        if (currentVolume_perc > ui->volumeSlider->minimum() + vol_step) {
            currentVolume_perc -= vol_step;
        }
    }



    if (currentVolumeOrig_perc != currentVolume_perc) {

        volumeChanged(currentVolume_perc);
    }
}

void GUI_SimplePlayer::setupVolButton(int percent) {

	QString butFilename = Helper::getIconPath() + "vol_";

    if (percent <= 1) {
        butFilename += QString("mute") + ".png";
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

	ui->btn_mute->setIcon(QIcon(butFilename));

}

void GUI_SimplePlayer::muteButtonPressed() {

	if (m_mute) {
		m_mute = false;

		ui->volumeSlider->setEnabled(true);
		setupVolButton(ui->volumeSlider->value());

		emit sig_volume_changed(ui->volumeSlider->value());
	}

	else {
		m_mute = true;
		ui->volumeSlider->setEnabled(false);
		ui->btn_mute->setIcon(QIcon(Helper::getIconPath() + "vol_mute.png"));

		setupVolButton(0);
		emit sig_volume_changed(0);
	}

	m_trayIcon->switch_mute_unmute(m_mute);

}

/** VOLUME END **/

