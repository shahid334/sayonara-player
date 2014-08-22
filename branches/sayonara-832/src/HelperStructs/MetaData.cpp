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
		pos_sec = 0;	
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

MetaData::MetaData(const MetaData& md) : LibraryItem(md){

        id = md.id;
        artist_id = md.artist_id;
        album_id = md.album_id;
        title = md.title;
        artist = md.artist;
        album = md.album;
        rating = md.rating;
		pos_sec = md.pos_sec;	
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

MetaData::~MetaData(){

}

void MetaData::print(){
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

bool MetaData::fromVariant(const QVariant& v, MetaData& md){

	if(! v.canConvert<MetaData>() ){
		return false;
	}

	md = v.value<MetaData>() ;
	return true;
}

bool MetaData::operator==(const MetaData& md){
	return this->is_equal(md, true);
}


bool MetaData::is_equal(const MetaData& md, bool case_sensitive){
	QString my_filepath = this->filepath.trimmed();
	QString their_filepath = md.filepath.trimmed();

	if( case_sensitive) {

		return (my_filepath.compare(their_filepath) == 0);
	}

	// else
		return my_filepath.compare(their_filepath, Qt::CaseInsensitive);
}



MetaDataList::MetaDataList() : 
	vector<MetaData>() 
	{ }

MetaDataList::MetaDataList(int n_elems) : 
	vector<MetaData>(n_elems)
	{ }

MetaDataList::~MetaDataList(){

}

void MetaDataList::setCurPlayTrack(uint idx){

   for(uint i=0; i<size(); i++){
       this->at(i).pl_playing = (i == idx);
   }
}

bool MetaDataList::contains(const MetaData& md, bool cs){
    QString filepath;

    if(cs){
        filepath = md.filepath.trimmed();

        for(uint i=0; i<size(); i++){

			if( this->at(i).is_equal(md, cs) ){
				return true;
			}

        }
    }

    return false;
}


void MetaDataList::insert(const MetaData& md, uint pos){

    if(pos >= size()){
        push_back(md);
        return;
    }

    uint sz = size();

    // copy last element
    push_back(at(sz - 1));
    sz++;

    // s
    for(uint j= sz-2; j>pos; j--){
        at(j) = at(j-1);
    }

    // replace
    at(pos) = md;
}
