/* CoverDownloader.h */

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
 * CoverDownloader.h
 *
 *  Created on: Jun 29, 2011
 *      Author: luke
 */



/*
 * DON'T TOUCH IT! DON'T COMMENT IT!
 * I WAS YOUNG AND NEEDED THE FUNCTIONALITY
 * */

#ifndef COVERDOWNLOADER_H_
#define COVERDOWNLOADER_H_

#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include "HelperStructs/WebAccess.h"

#include <curl/curl.h>

#include <QString>
#include <QStringList>
#include <QImage>
#include <QUrl>
#include <QDebug>
#include <QRegExp>
#include <QDir>


using namespace std;


QStringList calc_adresses_from_webpage(uint num, QString& qwebpage) {

	QStringList adresses;
	if (qwebpage.size() == 0) {
		cout << "No webpage found" << endl;
		return adresses;
	}


	int search_start = 0;

	while (true) {

		QString search_string = QString(".jpg");

		int loc_end = qwebpage.indexOf(search_string, search_start);
		if (loc_end == -1) {
			search_string = QString(".png");

			loc_end = qwebpage.indexOf(search_string, search_start);
		}

		if (loc_end != -1) {
			loc_end += 4;

			QString tmp_str = "";
			int tmp_idx = loc_end - 1;

			while (!tmp_str.startsWith("http") && tmp_idx > search_start) {
				tmp_str.prepend(qwebpage.at(tmp_idx));
				tmp_idx--;
			}

			bool adress_found = (tmp_str.startsWith("http"));
			if (adress_found) {
				QString adress = tmp_str;
				QUrl url(adress);

				if (!adresses.contains(adress) && url.isValid()) {
					adresses.push_back(adress);
					if((uint) adresses.size() >= num) break;
				}
			}

			search_start = loc_end;
		}

		else return adresses;
	}

	return adresses;
}

QStringList call_and_parse(QString url, int num_adresses){

	QString content;
	bool success = WebAccess::read_http_into_str(url, content);

	QStringList cover_adresses;
	if(success){
		cover_adresses = calc_adresses_from_webpage(num_adresses, content);
	}
	else{
		qDebug() << "could not get cover adresses from " << url;
	}

	return cover_adresses;
}

QStringList call_and_parse_artist(QString artist, int num_adresses, int cover_source){

	QString url_adress;

	switch(cover_source){

		case COV_SRC_GOOGLE:
			url_adress = Helper::calc_google_artist_adress(artist);
			break;

		case COV_SRC_LFM:
			url_adress = Helper::calc_lfm_artist_adress(artist);
			break;

		default: break;

	}

	return call_and_parse(url_adress, num_adresses);
}

QStringList call_and_parse_album(QString artist, QString album,	int num_adresses, int cover_source) {

	QString url_adress;

	switch(cover_source){

		case COV_SRC_GOOGLE:
			url_adress = Helper::calc_google_album_adress(artist, album);
			break;

		case COV_SRC_LFM:
			url_adress = Helper::calc_lfm_album_adress(artist, album);
			break;

		default: break;
	}

	return call_and_parse(url_adress, num_adresses);
}



// downloads num_covers_to_fetch covers out from adresses. If size of adresses is smaller, then the
// max number of covers will be fetched
// result images are stored in vec_images
uint download_covers(QStringList adresses, uint num_covers_to_fetch, vector<QImage>& vec_images, bool save=false) {

    vec_images.clear();
	uint found = 0;

	// if no adresses, try to fetch some
	if (adresses.size() == 0) {
		return 0;
	}

	// download
	for (int i = adresses.size() - 1; i >= 0; i--) {

		QImage img;
		bool success = WebAccess::read_http_into_img(adresses[i], img);

		if (!success) continue;

		if(save){

			QString target_filename = Helper::getSayonaraPath() +
					QDir::separator() +
					"tmp" +
					QDir::separator() +
					"image_" + QString::number(adresses.size()-i) + ".jpg";

			img.save(target_filename);
		}

		vec_images.push_back(img);
		found++;

		if (found >= num_covers_to_fetch)
			break;

	} // for all cover adresses

	// no covers could be fetched
	if (vec_images.size() == 0) {
		vec_images.push_back(QImage(Helper::getIconPath() + "append.png"));
	}

    return found;
}

#endif /* COVERDOWNLOADER_H_ */
