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



#include "HelperStructs/MetaData.h"

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
        filepath = "";
        track_num = 0;
        bitrate = 0;
        is_extern = false;
        radio_mode = RadioModeOff;
        filesize = 0;
        comment = "";
        discnumber = 0;
        n_discs = 0;

        pl_selected = false;
        pl_playing = false;
        pl_dragged = false;

        is_disabled = false;

}

MetaData::MetaData(const MetaData & md){
    id = md.id;
    artist_id = md.artist_id;
    album_id = md.album_id;
    title = md.title;
    artist = md.artist;
    album = md.album;
    rating = md.rating;
    length_ms = md.length_ms;
    year = md.year;
    filepath = md.filepath;
    track_num = md.track_num;
    bitrate = md.bitrate;
    is_extern = md.is_extern;
    radio_mode = md.radio_mode;
    filesize = md.filesize;
    comment = md.comment;
    discnumber = md.discnumber;
    n_discs = md.n_discs;

    pl_selected = md.pl_selected;
    pl_playing = md.pl_playing;
    pl_dragged = md.pl_dragged;

    is_disabled = md.is_disabled;
}


MetaData::~MetaData() {

}

void MetaData::print() const {
	qDebug() << title
             << " by " << artist
             << " from " << album
             << " (" << length_ms << " m_sec) :: " << filepath;
 
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


bool MetaData::is_equal(const MetaData& md) const {

    QString my_filepath = filepath.trimmed();
	QString their_filepath = md.filepath.trimmed();

#ifdef Q_OS_UNIX
		return (my_filepath.compare(their_filepath) == 0);
#else
		return (my_filepath.compare(their_filepath, Qt::CaseInsensitive) == 0);
#endif

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

    if(_cur_played_track >= 0){
        (this->data() + _cur_played_track)->pl_playing = false;
    }

    if(idx < 0) return;

    _cur_played_track = idx;
    (this->data() + _cur_played_track)->pl_playing = true;
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

int MetaDataList::findTrack(int id) const {

	if(id == -1) return -1;

    MetaDataList::const_iterator it;
    int idx = 0;
    for(it = this->begin(); it != this->end(); it++, idx++) {
		if(it->id == id) {
            return idx;
        }
    }

    return -1;
}

int MetaDataList::findTrack(const QString& path) const {

    MetaDataList::const_iterator it;

    int idx = 0;
    for(it = this->begin(); it != this->end(); it++, idx++) {

#ifdef Q_OS_UNIX
        if(it->filepath.compare(path, Qt::CaseSensitive) == 0){
#else
        if(it->filepath.compare(path, Qt::CaseInsensitive) == 0){
#endif
            return idx;
        }
    }

    return -1;
}


QStringList MetaDataList::toStringList() const {

    QStringList lst;
    MetaDataList::const_iterator it;

    for(it = this->begin(); it != this->end(); it++) {

        if(it->id >= 0) {
            lst << QString::number(it->id);
        }

        else{
            lst << it->filepath;
        }
    }

    return lst;
}


LastTrack::LastTrack() : MetaData(){

}

LastTrack::LastTrack(const MetaData& md) :
    MetaData(md){

    pos_sec = 0;
}

LastTrack::LastTrack(const LastTrack& lt) :
    MetaData(lt){

    pos_sec = 0;
}

LastTrack::~LastTrack(){

}

QVariant LastTrack::toVariant(const LastTrack& md) {

    QVariant v;
    v.setValue<LastTrack>(md);

    return v;
}

bool LastTrack::fromVariant(const QVariant& v, LastTrack& md) {

    if(! v.canConvert<LastTrack>() ) {
        return false;
    }

    md = v.value<LastTrack>() ;
    return true;
}
