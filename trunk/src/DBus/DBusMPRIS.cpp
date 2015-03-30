/* DBusMPRIS.cpp */

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



#include "src/DBus/DBusMPRIS.h"
#include "DBus/org_mpris_media_player2_adaptor.h"
#include "DBus/org_mpris_media_player2_player_adaptor.h"

#include <QDebug>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QStringList>
#include "CoverLookup/CoverLocation.h"

DBusAdaptor::DBusAdaptor(QObject* parent) :
	QObject(parent)
{

}

DBusAdaptor::~DBusAdaptor(){

}



void DBusAdaptor::create_message(QString name, QVariant val){

	QDBusMessage sig;
	QVariantMap map;
	QVariantList args;

	map.insert(name, val);
	args << _dbus_service << map << QStringList();


	sig = QDBusMessage::createSignal(_object_path, _dbus_path, "PropertiesChanged");
	sig.setArguments(args);

	QDBusConnection::sessionBus().send(sig);

}





DBusMPRIS::MediaPlayer2::MediaPlayer2(QObject *parent) :
	DBusAdaptor(parent),
	SayonaraClass()
{

	_play_manager = PlayManager::getInstance();

	_object_path = "/org/mpris/MediaPlayer2";
	_dbus_service = "org.mpris.MediaPlayer2";
	_service_name = "org.mpris.MediaPlayer2.sayonara";
	_dbus_path = "org.freedesktop.DBus.Properties";

	_pos = 0;
	_volume = 1.0;


	connect(_play_manager, SIGNAL(sig_playlist_changed(int)),
			this, SLOT(playlist_len_changed(int)));
	connect(_play_manager, SIGNAL(sig_playstate_changed(PlayManager::PlayState)),
			this, SLOT(playstate_changed(PlayManager::PlayState)));
	connect(_play_manager, SIGNAL(sig_track_changed(const MetaData&)),
			this, SLOT(track_changed(const MetaData&)));
	connect(_play_manager, SIGNAL(sig_track_idx_changed(int)),
			this, SLOT(track_idx_changed(int)));


	new OrgMprisMediaPlayer2Adaptor(this);
	new OrgMprisMediaPlayer2PlayerAdaptor(this);

	if (!QDBusConnection::sessionBus().registerService(_service_name)) {
		qDebug()	<< "Failed to register" << QString(_service_name)
					<< "on the session bus";
		return;
	}

	QDBusConnection::sessionBus().registerObject(_object_path, this);
	create_message("DesktopEntry", QString("sayonara"));

	REGISTER_LISTENER(Set::Engine_Vol, volume_changed);
	REGISTER_LISTENER(Set::Engine_CurTrackPos_s, position_changed);
}


DBusMPRIS::MediaPlayer2::~MediaPlayer2()
{
	QDBusConnection::sessionBus().unregisterObject(_object_path);
	QDBusConnection::sessionBus().unregisterService(_service_name);
}


bool DBusMPRIS::MediaPlayer2::CanQuit()
{
	return true;
}

bool DBusMPRIS::MediaPlayer2::CanRaise()
{
	return true;
}

bool DBusMPRIS::MediaPlayer2::HasTrackList(){
	return false;
}

QString DBusMPRIS::MediaPlayer2::Identity(){
	return "sayonara";
}

QString DBusMPRIS::MediaPlayer2::DesktopEntry(){
    return "sayonara";
}

QStringList DBusMPRIS::MediaPlayer2::SupportedUriSchemes(){
	QStringList uri_schemes;
	uri_schemes << "file"
				<< "http"
				<< "cdda"
				<< "smb"
				<< "sftp";

	return uri_schemes;
}

QStringList DBusMPRIS::MediaPlayer2::SupportedMimeTypes(){
	QStringList mimetypes;
	mimetypes   << "audio/mpeg"
				<< "audio/ogg";

	return mimetypes;
}

bool DBusMPRIS::MediaPlayer2::CanSetFullscreen(){
	return false;
}

bool DBusMPRIS::MediaPlayer2::Fullscreen(){
	return false;
}


void DBusMPRIS::MediaPlayer2::SetFullscreen(bool b){

}


void DBusMPRIS::MediaPlayer2::Quit(){

}

void DBusMPRIS::MediaPlayer2::Raise(){

}






/*** mpris.mediaplayer2.player ***/


QString DBusMPRIS::MediaPlayer2::PlaybackStatus(){
	//Playing, Paused, Stopped
	return _playback_status;
}

QString DBusMPRIS::MediaPlayer2::LoopStatus(){
	return "None";
}

double DBusMPRIS::MediaPlayer2::Rate(){
	return 1.0;
}

bool DBusMPRIS::MediaPlayer2::Shuffle(){
	return false;
}

QVariantMap DBusMPRIS::MediaPlayer2::Metadata(){
	QVariantMap map;
	QVariant var;
	var.setValue<qlonglong>(_md.length_ms * 1000);

	map["mpris:trackid"] = QVariant(1);
	map["mpris:length"] = var;
	map["xesam:title"] = _md.title;
	map["xesam:album"] = _md.album;
	map["xesam:artist"] = _md.artist;
	map["mpris:artUrl"] = QString("file://") + CoverLocation::get_cover_location(_md).cover_path;

	return map;
}

double DBusMPRIS::MediaPlayer2::Volume(){
	return _volume;
}

qint64 DBusMPRIS::MediaPlayer2::Position(){
	return _pos;
}

double DBusMPRIS::MediaPlayer2::MinimumRate(){
	return 1.0;
}

double DBusMPRIS::MediaPlayer2::MaximumRate(){
	return 1.0;
}

bool DBusMPRIS::MediaPlayer2::CanGoNext(){
	return _can_next;
}

bool DBusMPRIS::MediaPlayer2::CanGoPrevious(){
	return _can_previous;
}

bool DBusMPRIS::MediaPlayer2::CanPlay(){
	return true;
}

bool DBusMPRIS::MediaPlayer2::CanPause(){
	return true;
}

bool DBusMPRIS::MediaPlayer2::CanSeek(){
	return false;
}

bool DBusMPRIS::MediaPlayer2::CanControl(){
	return true;
}


void DBusMPRIS::MediaPlayer2::Next(){
	_play_manager->next();
}


void DBusMPRIS::MediaPlayer2::Previous(){

	_play_manager->previous();
}

void DBusMPRIS::MediaPlayer2::Pause(){
	_play_manager->pause();
}


void DBusMPRIS::MediaPlayer2::PlayPause(){
	_play_manager->play_pause();
}

void DBusMPRIS::MediaPlayer2::Stop(){
	_play_manager->stop();
}

void DBusMPRIS::MediaPlayer2::Play(){

	_playback_status = "Playing";

	create_message("PlaybackStatus", _playback_status);
}

void DBusMPRIS::MediaPlayer2::Seek(qlonglong offset){

}

void DBusMPRIS::MediaPlayer2::SetPosition(const QDBusObjectPath& trackId, qlonglong offset){

}

void DBusMPRIS::MediaPlayer2::OpenUri(const QString& uri){

}

void DBusMPRIS::MediaPlayer2::SetLoopStatus(QString status){

}

void DBusMPRIS::MediaPlayer2::SetRate(double rate){

}

void DBusMPRIS::MediaPlayer2::SetShuffle(bool shuffle){

}

void DBusMPRIS::MediaPlayer2::SetVolume(double volume){
	_settings->set(Set::Engine_Vol, (int) volume);

}



void DBusMPRIS::MediaPlayer2::volume_changed(){

	double volume =_settings->get(Set::Engine_Vol);
	create_message("Volume", volume);
}


void DBusMPRIS::MediaPlayer2::position_changed(){

	_pos = _settings->get(Set::Engine_CurTrackPos_s) * 1000000;
	QVariant pos_var;
	pos_var.setValue<qint64>(_pos);
	//create_message("Position", pos_var);
}


void DBusMPRIS::MediaPlayer2::track_idx_changed(int idx){

	_can_previous = (idx > 0);
	_can_next = (idx < _len_playlist - 1);

	create_message("CanGoNext", _can_next);
	create_message("CanGoPrevious", _can_previous);

	_cur_idx = idx;
}

void DBusMPRIS::MediaPlayer2::playlist_len_changed(int len){

	_can_next = (_cur_idx < len - 1 && _cur_idx >= 0);

	create_message("CanGoNext", _can_next);

	_len_playlist = len;


}

void DBusMPRIS::MediaPlayer2::track_changed(const MetaData& md){
	_md = md;

	QVariantMap map = Metadata();
	create_message("Metadata", map);
	Play();

}

void DBusMPRIS::MediaPlayer2::playstate_changed(PlayManager::PlayState state){

	QString playback_status;

	switch(state){
		case PlayManager::PlayState_Stopped:
			playback_status = "Stopped";
			break;
		case PlayManager::PlayState_Playing:
			playback_status = "Playing";
			break;
		case PlayManager::PlayState_Paused:
			playback_status = "Paused";
			break;
		default:
			playback_status = "Stopped";
			break;
	}

	_playback_status = playback_status;

	create_message("PlaybackStatus", playback_status);
}


