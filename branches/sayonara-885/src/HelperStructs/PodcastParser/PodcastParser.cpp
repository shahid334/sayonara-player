/* PodcastParser.cpp */

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



/*
 *
 * valid_document = first tag LIKE <rss version="2.0">
 * valid_item = channel.item.enclosore::(type).contains(audio) > channel.item.enclosure == NULL
 *
 *
 * MetaData.title = channel.item.title
 * MetaData.artist = itunes:author > channel.item.author
 * MetaData.album = channel.title
 * MetaData.year = channel.item.pubDate.year > channel.pubDate.yeicon.url = > item.image.url > itunes:image (1400x1400) px 
 *
 *
 *
 * */
#include <QString>
#include <QDomDocument>
#include "HelperStructs/Helper.h"
#include "HelperStructs/WebAccess.h"
#include "HelperStructs/PodcastParser/PodcastParser.h"
#include "CoverLookup/CoverLocation.h"

int find_year(QString str) {

    int idx = str.indexOf(QRegExp("[0-9]{4,4}"));

    if(idx >= 0) {
        return str.mid(idx, 4).toInt();
    }

    return 0;
}

bool  Podcast::parse_podcast_xml_file_content(const QString& content, MetaDataList& v_md) {

    v_md.clear();

    QDomDocument doc("PodcastFile");
    doc.setContent(content);

    QDomElement docElement = doc.documentElement();
    QDomNode entry = docElement.firstChildElement("channel");

    if(!entry.hasChildNodes()) return false;

    QString author;
    QStringList categories;
    QString album;
	QImage img;

    for(int c = 0; c<entry.childNodes().size(); c++) {

        QDomNode channel_child = entry.childNodes().at(c);
        QString nodename = channel_child.nodeName();
        QDomElement e = channel_child.toElement();

        if(!nodename.compare("title", Qt::CaseInsensitive)) {
            album = e.text();
        }

        else if(!nodename.compare("itunes:author", Qt::CaseInsensitive)) {
            author = e.text();
        }

        else if(!nodename.compare("itunes:category", Qt::CaseInsensitive)) {

            QStringList genres = e.text().split(QRegExp(",|/|;|\\."));
            for(int i=0; i<genres.size(); i++) {
                genres[i] = genres[i].trimmed();
            }

            categories.append(genres);
        }

		else if(!nodename.compare("image", Qt::CaseInsensitive) && img.isNull()) {
            if(!channel_child.hasChildNodes()) continue;

            for(int i=0; i<channel_child.childNodes().size(); i++) {
                QDomNode item_child = channel_child.childNodes().at(i);
                QString ic_nodename = item_child.nodeName();
                QDomElement ic_e = item_child.toElement();
                if(!ic_nodename.compare("url", Qt::CaseInsensitive)) {
					QString cover_path = CoverLocation::get_cover_location(album, author).cover_path;
                    QString img_url = ic_e.text();
                    QImage img;

                    bool success = WebAccess::read_http_into_img(img_url, &img);
                    if(!success && !img.isNull()) continue;
					img.save( cover_path );
                }
            }
        }

        else if(!nodename.compare("itunes:image", Qt::CaseInsensitive)) {
            QString img_url = e.attribute("href");

			bool success = WebAccess::read_http_into_img(img_url, &img);
            if(!success && !img.isNull()) continue;
        }

        // item
        else if(!nodename.compare("item", Qt::CaseInsensitive)) {

            if(!channel_child.hasChildNodes()) continue;
            MetaData md;
            md.genres.push_back("Podcasts");
            md.genres.append(categories);

            md.album = QString::fromUtf8(album.toStdString().c_str());
            md.artist = QString::fromUtf8(author.toStdString().c_str());

            for(int i=0; i<channel_child.childNodes().size(); i++)
            {

                QDomNode item_child = channel_child.childNodes().at(i);
                QString ic_nodename = item_child.nodeName();
                QDomElement ic_e = item_child.toElement();

                if(!ic_nodename.compare("title", Qt::CaseInsensitive)) {
                    md.title = QString::fromUtf8(ic_e.text().toStdString().c_str());
                }

                else if(!ic_nodename.compare("link", Qt::CaseInsensitive)) {
                    md.filepath = ic_e.text();
                }

                else if(!ic_nodename.compare("enclosure", Qt::CaseInsensitive)) {
                    md.filesize = ic_e.attribute("length").toLong();
                }

                else if(!ic_nodename.compare("author") && md.artist.size() == 0) {
                    md.artist = QString::fromUtf8(ic_e.text().toStdString().c_str());
                }

                else if(!ic_nodename.compare("itunes:author", Qt::CaseInsensitive)) {
                    md.artist = QString::fromUtf8(ic_e.text().toStdString().c_str());
                }

                else if(!ic_nodename.compare("itunes:duration", Qt::CaseInsensitive)) {

                    QString text = ic_e.text();
                    QStringList lst = text.split(":");
                    int len = 0;
                    for(int i=lst.size() -1; i>=0; i--) {
                        if(i == lst.size() -1)
                            len += lst[i].toInt();
                        else if(i == lst.size() -2) {
                            len += lst[i].toInt() * 60;
                        }
                        else if(i == lst.size() -3) {
                            len += lst[i].toInt() * 3600;
                        }
                    }

                    md.length_ms = len * 1000;
                }


                else if(!ic_nodename.compare("pubDate", Qt::CaseInsensitive) ||
                        !ic_nodename.compare("dc:date", Qt::CaseInsensitive)) {
                    md.year = find_year(ic_e.text());
                }
            } // foreach item entry

            if(md.filepath.size() > 0)
                v_md.push_back(md);
        } // item
    }

	if( !img.isNull() ){
		QString cover_path = CoverLocation::get_cover_location(album, author).cover_path;
		img.save( cover_path );
	}

    return (v_md.size() > 0);
}


bool Podcast::parse_podcast_xml_file(QString podcast_filename, MetaDataList& v_md) {


    QString content;

	if(Helper::is_www(podcast_filename)){
		Helper::read_http_into_str(podcast_filename, &content);
	}

	else{
		Helper::read_file_into_str(podcast_filename, &content);
	}

    return parse_podcast_xml_file_content(content, v_md);



}
