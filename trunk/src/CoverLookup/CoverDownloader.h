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

#include "HelperStructs/WebAccess.h"

#include <curl/curl.h>

#include <QStringList>
#include <QImage>
#include <QUrl>
#include <QDebug>
#include <QRegExp>
#include <QDir>

QStringList cov_calc_adresses_from_webpage(uint num, QString& webpage) {

	QStringList adresses;
    if (webpage.size() == 0) {
        qDebug() << "No webpage found" << endl;
		return adresses;
	}

    uint n_covers = 0;
    int idx=40000;

    //qDebug() << webpage;

    while(n_covers < num){
        QString re_str("(https://encrypted-tbn)(\\S)+(\")");
        QRegExp re(re_str);
        idx = re.indexIn(webpage, idx);
        if(idx == -1) break;
        QString str = re.cap(0);

        idx += str.length();
        str.remove("\"");
        adresses << str;
        n_covers++;
    }

    qDebug() << "Got Adresses";

	return adresses;
}

QStringList cov_call_and_parse(QString url, int num_adresses){

    qDebug() << "Url = " << url;

	QString content;
    bool success = WebAccess::read_http_into_str(url, &content);

	QStringList cover_adresses;
	if(success){
        cover_adresses = cov_calc_adresses_from_webpage(num_adresses, content);
	}
	else{
		qDebug() << "could not get cover adresses from " << url;
	}

	return cover_adresses;
}


bool cov_download_cover(QString adress, QImage* img) {

    qDebug() << "Cover adress = " << adress;

    return WebAccess::read_http_into_img(adress, img);

}

#endif /* COVERDOWNLOADER_H_ */
