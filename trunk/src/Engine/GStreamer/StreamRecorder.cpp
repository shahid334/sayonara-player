/* StreamRecorder.cpp */

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



#include "Engine/GStreamer/StreamRecorder.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Parser/PlaylistParser.h"
#include "HelperStructs/Tagging/id3.h"

#include <QFile>
#include <QDir>
#include <QDateTime>



static QString get_time_str() {

    QString time_str;
    QDateTime cur = QDateTime::currentDateTime();

    QString weekday = cur.date().longDayName(cur.date().dayOfWeek() );
    QString year = QString::number(cur.date().year());
    QString month = cur.date().shortMonthName(cur.date().month());
    QString day = QString("%1").arg(cur.date().day(), 2, 10, QLatin1Char('0'));
    QString hr = QString("%1").arg(cur.time().hour(), 2, 10, QLatin1Char('0'));
    QString min = QString("%1").arg(cur.time().minute(), 2, 10, QLatin1Char('0'));

    time_str = weekday + "_" + year + "-" + month + "-" + day + "_" + hr + ":" + min;
    return time_str;
}



StreamRecorder::StreamRecorder(QObject *parent) :
	QObject(parent),
	SayonaraClass()
{
    _session_path = get_time_str();
    _record_on = false;
	_idx = 1;

    QDir d(Helper::getSayonaraPath());

    // delete old stream ripper files
    QStringList lst = d.entryList(Helper::get_soundfile_extensions());
	for( const QString& str : lst) {
		qDebug() << "Remove " << d.absolutePath() + QDir::separator() + str;
        QFile f(d.absolutePath() + QDir::separator() + str);
        f.remove();
    }
}


StreamRecorder::~StreamRecorder() {

}


void StreamRecorder::new_session(){

    _md.title = "";
	_session_path = get_time_str();
	_session_collector.clear();
    _sr_recording_dst = "";
	_idx = 1;

    qDebug() << "New session: " << _session_path;
}



void StreamRecorder::changeTrack(const MetaData& md) {

    if(md.title == _md.title) return;
	save();
	
	_md = md;

    if(!Helper::is_www(md.filepath())) {
        _sr_recording_dst = "";
        return;
	}
	
	QString title = QString("%1").arg(_idx, 3, 10, QLatin1Char('0')) + "_" + md.title;
	_idx++;

	title.replace(" ", "_");
	title.replace("/", "_");
	title.replace("\\", "_");

	QString sr_path = _settings->get(Set::Engine_SR_Path);
	QString session_path = check_session_path(sr_path);

	_session_playlist_name = session_path + "/playlist.m3u";
	_sr_recording_dst = session_path + "/" + title + ".mp3";
}


bool  StreamRecorder::save() {

    if(!QFile::exists(_sr_recording_dst)){
        return false;
    }

	qDebug() << "Finalize file " << _sr_recording_dst;

	_md.set_filepath(_sr_recording_dst);
	ID3::setMetaDataOfFile(_md);
	_session_collector.push_back(_md);

	PlaylistParser::save_playlist(_session_playlist_name, _session_collector, true);
	
    return true;
}


QString StreamRecorder::check_session_path(QString sr_path) {

	bool create_session_path =_settings->get(Set::Engine_SR_SessionPath);

    if(!create_session_path) return sr_path;

    if(!QFile::exists(sr_path + QDir::separator() + _session_path)) {

        QDir dir(sr_path);
        dir.mkdir(_session_path);
    }

    return sr_path + QDir::separator() + _session_path;
}


void StreamRecorder::activate(bool b){

    if(b == _record_on) return;

    qDebug() << "Activate";

    if(b){
        new_session();
    }

    else{
        save();
    }

    _record_on = b;
}

QString StreamRecorder::get_dst_file(){
    if(!_record_on) return "";

    return _sr_recording_dst;
}
