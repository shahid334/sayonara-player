/* MetaData.cpp */

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



#include "HelperStructs/MetaData/MetaData.h"
#include <QDir>
#include <QUrl>

MetaData::MetaData() : LibraryItem() {

        id = -1;
        artist_id = -1;
        album_id = -1;
        title = "";
        artist = "";
        album = "";
        rating = 0;
        length_ms = 0;
        year = 0;
		_filepath = "";
        track_num = 0;
        bitrate = 0;
        is_extern = false;
		_radio_mode = RadioModeOff;
        filesize = 0;
        comment = "";
        discnumber = 0;
        n_discs = 0;

        pl_selected = false;
        pl_playing = false;
        pl_dragged = false;

        is_disabled = false;
}

MetaData::MetaData(const MetaData & md) : LibraryItem(md){
    id = md.id;
    artist_id = md.artist_id;
    album_id = md.album_id;
    title = md.title;
    artist = md.artist;
    album = md.album;
    rating = md.rating;
    length_ms = md.length_ms;
    year = md.year;
	_filepath = md.filepath();
    track_num = md.track_num;
    bitrate = md.bitrate;
    is_extern = md.is_extern;
	_radio_mode = md.radio_mode();
    filesize = md.filesize;
    comment = md.comment;
    discnumber = md.discnumber;
    n_discs = md.n_discs;

    pl_selected = md.pl_selected;
    pl_playing = md.pl_playing;
    pl_dragged = md.pl_dragged;

    is_disabled = md.is_disabled;
	is_lib_selected = md.is_lib_selected;
}


MetaData::~MetaData() {

}

void MetaData::print() const {
	qDebug() << title
             << " by " << artist
             << " from " << album
			 << " (" << length_ms << " m_sec) :: " << _filepath;
 
}

QVariant MetaData::toVariant(const MetaData& md) {

	QVariant v;
	v.setValue<MetaData>(md);

	return v;
}

bool MetaData::fromVariant(const QVariant& v, MetaData& md) {

	if(! v.canConvert<MetaData>() ) {
		return false;
	}

	md = v.value<MetaData>() ;
	return true;
}

bool MetaData::operator==(const MetaData& md) const {
    return this->is_equal(md);
}


bool MetaData::operator!=(const MetaData& md) const {
	return !(this->is_equal(md));
}



bool MetaData::is_equal(const MetaData& md) const {

	QDir first_path(_filepath);
	QDir other_path(md.filepath());

	QString s_first_path = first_path.absolutePath();
	QString s_other_path = other_path.absolutePath();



#ifdef Q_OS_UNIX
		return (s_first_path.compare(s_other_path) == 0);
#else
		return (s_first_path.compare(s_other_path, Qt::CaseInsensitive) == 0);
#endif

}

bool MetaData::is_equal_deep(const MetaData& md) const{

	return ( (id == md.id)  &&
	( artist_id == md.artist_id ) &&
	( album_id == md.album_id ) &&
	( title == md.title ) &&
	( artist == md.artist ) &&
	( album == md.album ) &&
	( rating == md.rating ) &&
	( length_ms == md.length_ms ) &&
	( year == md.year ) &&
	( filepath() == md.filepath() ) &&
	( track_num == md.track_num ) &&
	( bitrate == md.bitrate ) &&
	( is_extern == md.is_extern ) &&
	( _radio_mode == md.radio_mode() ) &&
	( filesize == md.filesize ) &&
	( comment == md.comment ) &&
	( discnumber == md.discnumber ) &&
	( n_discs == md.n_discs ) &&

	( pl_selected == md.pl_selected ) &&
	( pl_playing == md.pl_playing ) &&
	( pl_dragged == md.pl_dragged ) &&

	( is_disabled == md.is_disabled )
	);
}

QString MetaData::filepath() const{
	return _filepath;
}


QString MetaData::set_filepath(QString filepath){

	bool is_local_path = false;

	#ifdef Q_OS_UNIX
		if(filepath.startsWith("/")){
			is_local_path = true;
		}
	#else
		if(filepath.contains(":\\") || filepath.contains("\\\\")){
			is_local_path = true;
		}
	#endif

	if(is_local_path){
		QDir dir(filepath);
		_filepath = dir.absolutePath();
		_radio_mode = RadioModeOff;
	}

	else if(filepath.contains("soundcloud.com")){
		_filepath = filepath;
		_radio_mode = RadioModeSoundcloud;
	}

	else{
		_filepath = filepath;
		_radio_mode = RadioModeStation;
	}

	return _filepath;
}

RadioMode MetaData::radio_mode() const {
	return _radio_mode;
}


MetaDataList::MetaDataList() : 
    QVector<MetaData>()
{
    _cur_played_track = -1;
}

MetaDataList::MetaDataList(int n_elems) : 
    QVector<MetaData>(n_elems)
{
    _cur_played_track = -1;
}

MetaDataList::MetaDataList(const MetaDataList& lst) :
    QVector(lst)
{
    _cur_played_track = lst._cur_played_track;
}

MetaDataList::~MetaDataList() {

}

void MetaDataList::setCurPlayTrack(int idx) {

	if(_cur_played_track >= 0 && _cur_played_track < this->size() ){
		(this->data() + _cur_played_track)->pl_playing = false;
	}

	else{
		_cur_played_track = -1;
	}

	if(idx < 0 || idx >= this->size()) {
		_cur_played_track = -1;
		return;
	}

	_cur_played_track = idx;

	(this->data() + _cur_played_track)->pl_playing = true;

}

int MetaDataList::getCurPlayTrack() const {
	return _cur_played_track;
}


bool MetaDataList::contains(const MetaData& md) const {

    MetaDataList::const_iterator it;
    for(it = this->begin(); it != this->end(); it++) {

        if( it->is_equal(md) ) {
            return true;
        }
    }

    return false;
}

QList<int> MetaDataList::findTracks(int id) const {

	QList<int> ret;

	if(id == -1) return ret;

    MetaDataList::const_iterator it;
    int idx = 0;
    for(it = this->begin(); it != this->end(); it++, idx++) {
		if(it->id == id) {
			ret << idx;
        }
    }

	return ret;
}

QList<int> MetaDataList::findTracks(const QString& path) const {

	QList<int> ret;
    MetaDataList::const_iterator it;

    int idx = 0;
    for(it = this->begin(); it != this->end(); it++, idx++) {

		QString filepath = it->filepath();

#ifdef Q_OS_UNIX
		if(filepath.compare(path, Qt::CaseSensitive) == 0){
#else
		if(filepath.compare(path, Qt::CaseInsensitive) == 0){
#endif
			ret << idx;
        }
    }

	return ret;
}


QStringList MetaDataList::toStringList() const {

    QStringList lst;
    MetaDataList::const_iterator it;

    for(it = this->begin(); it != this->end(); it++) {

		if( it->id >= 0 ) {
            lst << QString::number(it->id);
        }

        else{
			lst << it->filepath();
        }
    }

    return lst;
}




