/* MetaData.h */

/* Copyright (C) 2011  Lucio Carreras
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


/*
 * MetaData.h
 *
 *  Created on: Mar 10, 2011
 *      Author: luke
 */

#ifndef METADATA_H_
#define METADATA_H_

#define DROP_TYPE_ARTISTS 0
#define DROP_TYPE_ALBUMS 1
#define DROP_TYPE_TRACKS 2

#include "HelperStructs/globals.h"


#include <QString>
#include <QStringList>
#include <QPair>
#include <QDebug>
#include <QVariant>
#include <QMap>

#include <vector>



using namespace std;



struct MetaData {

private:


public:
    qint32 id;
    qint32 album_id;
    qint32 artist_id;
    QString title;
    QString artist;
    QString album;
    QStringList genres;
    qint8 rating;
    qint64 length_ms;
    qint16 year;
    QString filepath;
    qint16 track_num;
    qint32 bitrate;
    qint64 filesize;
    QString comment;
    qint8 discnumber;
    qint8 n_discs;

    bool is_extern;
    RadioMode radio_mode;

    bool pl_selected;
    bool pl_playing;
    bool pl_dragged;

    bool is_lib_selected;
    bool is_disabled;


    inline MetaData () {
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
        radio_mode = RadioOff;
        filesize = 0;
        comment = "";
        discnumber = 0;
        n_discs = -1;

        pl_selected = false;
        pl_playing = false;
        pl_dragged = false;

        is_lib_selected = false;
        is_disabled = false;
    }

    void print(){

        qDebug() << title
                 << " by " << artist
                 << " from " << album
                 << " (" << length_ms << " m_sec) :: " << filepath;
    }

    QVariant toVariant() const{

        QStringList list;

        QString tmpTitle = title;
        QString tmpArtist = artist;
        QString tmpAlbum = album;

        if(title.trimmed().size() == 0) tmpTitle = QString("(Unknown title)");
        if(artist.trimmed().size() == 0) tmpArtist = QString("");
        if(album.trimmed().size() == 0) tmpAlbum = QString("");

        list.push_back(tmpTitle);
        list.push_back(tmpArtist);
        list.push_back(tmpAlbum);
        list.push_back(QString::number(rating));
        list.push_back(QString::number(length_ms));
        list.push_back(QString::number(year));
        list.push_back(filepath);
        list.push_back(QString::number(track_num));
        list.push_back(QString::number(bitrate));
        list.push_back(QString::number(id));
        list.push_back(QString::number(album_id));
        list.push_back(QString::number(artist_id));
        list.push_back(QString::number(filesize));
        list.push_back(comment);
        list.push_back(QString::number(discnumber));
        list.push_back(QString::number(n_discs));
        list.push_back(genres.join(","));
        list.push_back(QString::number(   (is_extern) ? 1 : 0  ));
        list.push_back( QString::number(radio_mode) );
        list.push_back( (pl_playing) ? "1" : "0" );
        list.push_back( (pl_selected) ? "1" : "0" );
        list.push_back( (pl_dragged) ? "1" : "0" );
        list.push_back( (is_lib_selected) ? "1" : "0" );
        list.push_back( (is_disabled) ? "1" : "0");

        return list;
    }

    static bool fromVariant(QVariant v, MetaData& md){

        QStringList list = v.toStringList();

        if(list.size() < 24) return false;

        md.title =      list[0];
        md.artist =     list[1];
        md.album =      list[2];
        md.rating =     list[3].toInt();
        md.length_ms =  list[4].toULongLong();
        md.year =       list[5].toInt();
        md.filepath =   list[6];
        md.track_num =  list[7].toInt();
        md.bitrate =    list[8].toInt();
        md.id =         list[9].toInt();
        md.album_id =   list[10].toInt();
        md.artist_id =  list[11].toInt();
        md.filesize =   list[12].toInt();
        md.comment =    list[13];
        md.discnumber = list[14].toInt();
        md.n_discs =    list[15].toInt();
        md.genres =     list[16].split(",");
        md.is_extern = ( list[17] == "1" );
        md.radio_mode = (RadioMode) list[18].toInt();
        md.pl_playing = (list[19] == "1");
        md.pl_selected = (list[20] == "1");
        md.pl_dragged = (list[21] == "1");
        md.is_lib_selected = (list[22] == "1");
        md.is_disabled = (list[23] == "1");

        return true;
    }
};

class MetaDataList : public vector<MetaData>{

public:

    MetaDataList(){}
    ~MetaDataList(){
        clear();
    }

    void setCurPlayTrack(int idx){
        for(uint i=0; i<size(); i++){
            if((int) i == idx) this->at(i).pl_playing = true;
            this->at(i).pl_playing = false;
        }
    }

    bool contains(const MetaData& md, bool cs=false){
        QString filepath;

        if(cs){
            filepath = md.filepath.trimmed();

            for(uint i=0; i<size(); i++){
                MetaData md =  vector<MetaData>::at(i);
                QString filepath2 = md.filepath.trimmed();
                if(!filepath.compare(filepath2)) return true;

            }
        }

        else{

            filepath = md.filepath.toLower().trimmed();

            for(uint i=0; i<size(); i++){
                MetaData md =  vector<MetaData>::at(i);
                QString filepath2 = md.filepath.toLower().trimmed();
                if(!filepath.compare(filepath2)) return true;

            }
        }

        return false;
    }

    void insert_mid(const MetaData& md, int pos){

        if(pos < 0) pos = 0;

        if(pos >= (int) size()){
            push_back(md);
            return;
        }



        uint sz = size();

        // copy last element
        push_back(at(sz - 1));
        sz++;

        // s
        for(uint j= sz-2; j>(uint) pos; j--){
            at(j) = at(j-1);
        }

        // replace
        at(pos) = md;
    }
};

struct Artist{
    QString name;
    qint32	id;
    qint16	num_songs;
    qint16  num_albums;
    bool is_lib_selected;


    Artist(){
        id = -1;
        name = "";
        num_songs = 0;
        num_albums = 0;
        is_lib_selected = false;
    }

    QVariant toVariant(){
        QStringList list;
        QString tmpName = name;
        if(name.trimmed().size() == 0) tmpName = QString("(Unknown artist)");
        list.push_back(tmpName);
        list.push_back(QString::number(id));
        list.push_back(QString::number(num_songs));
        list.push_back(QString::number(num_albums));
        list.push_back((is_lib_selected ? "1" : "0"));
        return list;
    }

    void fromVariant(const QVariant& v){
        QStringList list = v.toStringList();
        if(list.size() < 4) return;
        name = list[0];
        id = list[1].toInt();
        num_songs = list[2].toInt();
        num_albums = list[3].toInt();
        is_lib_selected = ((list[4] == "1") ? true : false);
    }

};


struct Album{
    QString name;
    qint32	id;
    qint16 	num_songs;
    qint16 	length_sec;
    qint16	year;
    qint8 n_discs;
    QStringList artists;
    QList<int> discnumbers;

    bool is_splitted;
    bool is_sampler;
    bool is_lib_selected;


    Album(){
        name = "";
        id = -1;
        num_songs = 0;
        length_sec = 0;
        year = 0;
        n_discs = 1;
        is_splitted = false;
        is_sampler = false;
        is_lib_selected = false;
    }

    QVariant toVariant(){
        QStringList list;
        QString tmpName = "(Unknown album)";
        if(name.trimmed().size() > 0) tmpName = name;

        list.push_back(tmpName);
        if(artists.size() > 0){
            list.push_back(artists.join(","));
        }
        else list.push_back("");


        list.push_back(QString::number(id));
        list.push_back(QString::number(num_songs));
        list.push_back(QString::number(length_sec));
        list.push_back(QString::number(year));

        QStringList strl_discnumbers;
        foreach(int disc, discnumbers){
            strl_discnumbers << QString::number(disc);
        }

        list.push_back(strl_discnumbers.join(","));
        list.push_back(QString::number(n_discs));
        list.push_back( (is_splitted == true) ? "1" : "0" );
        list.push_back( (is_sampler == true) ? "1" : "0" );
        list.push_back( (is_lib_selected == true) ? "1" : "0" ) ;
        
        return list;
    }


    void fromVariant(const QVariant& v){
        QStringList list = v.toStringList();
        if(list.size() < 11) return;


        name =      list[0];
        QString tmp_artists = list[1];
        QStringList tmp_list = tmp_artists.split(',');
        artists =       tmp_list;
        id =            list[2].toInt();
        num_songs =     list[3].toInt();
        length_sec =    list[4].toLong();
        year =          list[5].toInt();

        QStringList strl_discnumbers = list[6].split(',');
        discnumbers.clear();
        foreach(QString disc, strl_discnumbers){
            discnumbers << disc.toInt();
        }

        n_discs =       list[7].toInt();
        is_splitted = (list[8] == "1");

        is_sampler =    (list[9] == "1");
        is_lib_selected = (list[10] == "1");
    }



};



struct CustomPlaylist{
    QString name;
    qint32 id;
    MetaDataList tracks;
    qint32 length;
    qint32 num_tracks;
    bool is_valid;
    CustomPlaylist(){
        is_valid = false;
        id = -1;
        name = "";
        length = 0;
        num_tracks = 0;
    }
};


typedef struct QList<Album> AlbumList;
typedef struct QList<Artist> ArtistList;

struct LastTrack{
    qint32 id;
    QString filepath;
    qint32 pos_sec;
    bool valid;

    LastTrack(){
        id = -1;
        filepath = "";
        pos_sec = -1;
        valid = false;
    }

    void reset(){
        id = -1;
        filepath = "";
        pos_sec = -1;
        valid = false;
    }

    QString toString(){

        QString str;
        str += QString::number(id) + ",";
        str += filepath + ",";
        str += QString::number(pos_sec);
        return str;
    }

    static LastTrack fromString(QString str){
        QStringList lst = str.split(",");
        LastTrack tr;
        if(lst.size() < 3) return tr;
        tr.id = lst[0].toInt();
        tr.filepath = lst[1];
        bool ok;
        tr.pos_sec = lst[2].toInt(&ok);

        if(!ok && lst.size() > 3){
            tr.pos_sec = lst[lst.size() - 1].toInt();
            tr.filepath.clear();
            for(int i=1; i<lst.size() - 1; i++){
                tr.filepath += lst[i];
            }
        }

        tr.valid = true;
        return tr;
    }
};


#endif /* METADATA_H_ */
