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
		ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
		emit sig_pause();
	}

    else {

		ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "pause.png"));
		emit sig_play();
	}

	m_playing = !m_playing;
	m_trayIcon->setPlaying(m_playing);
}

void GUI_Player::stopClicked(bool b) {


    ui->btn_play->setIcon(QIcon(Helper::getIconPath() + "play.png"));
    m_trayIcon->setPlaying(false);
	m_trayIcon->stop();
	m_playing = false;


    ui->lab_title->hide();
    ui->lab_sayonara->show();

    ui->lab_artist->hide();
    ui->lab_writtenby->show();

    ui->lab_album->hide();
    ui->lab_version->show();

    ui->lab_rating->hide();
    ui->lab_copyright->show();

    ui->songProgress->setValue(0);
	ui->songProgress->setEnabled(false);

	ui->curTime->setText("00:00");
	ui->maxTime->setText("00:00");


	this->setWindowTitle("Sayonara");

	set_std_cover( false );

	if(ui->btn_rec->isVisible() && ui->btn_rec->isChecked()) {
		ui->btn_rec->setChecked(false);
        emit sig_rec_button_toggled(false);
    }



	if(b) {
		MetaData md;
		m_settings->setLastTrack(md);

        emit sig_stop();
    }
}

void GUI_Player::backwardClicked(bool) {

   // ui->albumCover->setFocus();
    int cur_pos_sec =  (m_completeLength_ms * ui->songProgress->value()) / 100000;
	if(cur_pos_sec > 3) {
        setProgressJump(0);
    }

    else{
        emit sig_backward();
    }
}

void GUI_Player::forwardClicked(bool) {
    //ui->albumCover->setFocus();
	emit sig_forward();
}


void GUI_Player::sl_rec_button_toggled(bool b) {

    emit sig_rec_button_toggled(b);
}



/** PROGRESS BAR **/
void GUI_Player::total_time_changed(qint64 total_time) {

	QString length_str = Helper::cvtMsecs2TitleLengthString(total_time, true);
    m_completeLength_ms = total_time;
    ui->maxTime->setText(length_str);
}

void GUI_Player::jump_forward() {

	int percent = ui->songProgress->value();
    percent += 2;
    setProgressJump(percent);
	ui->songProgress->setValue(percent);

}

void GUI_Player::jump_backward() {
	int percent = ui->songProgress->value();
    percent -= 2;

    setProgressJump(percent);
	ui->songProgress->setValue(percent);
}

void GUI_Player::setProgressJump(int percent) {

    if(percent > 100 || percent < 0) {
        percent = 0;
    }

    long cur_pos_ms = (percent * m_metadata.length_ms) / 100;
    QString curPosString = Helper::cvtMsecs2TitleLengthString(cur_pos_ms);
    ui->curTime->setText(curPosString);

	emit sig_seek_rel(percent);
}

void GUI_Player::setCurrentPosition(quint32 pos_sec) {

    if (m_completeLength_ms != 0) {

		int newSliderVal = (pos_sec * 100000) / (m_completeLength_ms);

		if (!ui->songProgress->isSearching() && newSliderVal < ui->songProgress->maximum()) {

            ui->songProgress->setValue(newSliderVal);
		}
	}

	else if(pos_sec > m_completeLength_ms / 1000) {
    	ui->songProgress->setValue(0);
    }


	if(!ui->songProgress->isSearching()) {

        if(m_completeLength_ms != 0 && pos_sec > m_completeLength_ms) pos_sec = 0;

        QString curPosString = Helper::cvtMsecs2TitleLengthString(pos_sec * 1000);
        ui->curTime->setText(curPosString);
    }

}

/** PROGRESS BAR END **/

/** PLAYER BUTTONS END **/



/** VOLUME **/
void GUI_Player::setVolume(int vol) {
	ui->volumeSlider->setValue(vol);
	setupVolButton(vol);
	emit sig_volume_changed(vol);
}

void GUI_Player::volumeChanged(int volume_percent) {
	setupVolButton(volume_percent);
    ui->volumeSlider->setValue(volume_percent);
	emit sig_volume_changed(volume_percent);

    m_settings->setVolume(volume_percent);
}

void GUI_Player::volumeChangedByTick(int val) {


    int currentVolumeOrig_perc = this -> ui->volumeSlider->value();
    int currentVolume_perc = currentVolumeOrig_perc;
    int vol_step = m_trayIcon->get_vol_step();

    if (val > 0) {
        //increase volume
        if (currentVolume_perc < ui->volumeSlider->maximum() - vol_step) {
            currentVolume_perc += vol_step;
        }

        else currentVolumeOrig_perc = 100;
    }

    else if (val < 0) {
        //decrease volume
        if (currentVolume_perc > ui->volumeSlider->minimum() + vol_step) {
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

	QString butFilename = Helper::getIconPath() + "vol_";

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

	ui->btn_mute->setIcon(QIcon(butFilename));

}

void GUI_Player::muteButtonPressed() {

	if (m_mute) {

        setupVolButton(ui->volumeSlider->value());
		emit sig_volume_changed(ui->volumeSlider->value());
	}

	else {

        setupVolButton(0);
		emit sig_volume_changed(0);
	}

    m_mute = !m_mute;

    ui->volumeSlider->setDisabled(m_mute);
    m_trayIcon->setMute(m_mute);

    ui->volumeSlider->update();
}

/** VOLUME END **/


