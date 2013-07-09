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



#include "HelperStructs/WebAccess.h"
#include "CoverLookup/CoverDownloader.h"

#include <curl/curl.h>

#include <QString>
#include <QStringList>
#include <QImage>
#include <QUrl>
#include <QDebug>
#include <QRegExp>
#include <QDir>


QStringList CoverDownloader::cov_calc_adresses_from_webpage(uint num, QString& qwebpage) {

    QStringList adresses;
    if (qwebpage.size() == 0) {
        qDebug() << "No webpage found" << endl;
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

QStringList CoverDownloader::cov_call_and_parse(QString url, int num_adresses){

    QString content;
    bool success = WebAccess::read_http_into_str(url, content);

    QStringList cover_adresses;
    if(success){
        cover_adresses = cov_calc_adresses_from_webpage(num_adresses, content);
    }
    else{
        qDebug() << "could not get cover adresses from " << url;
    }

    return cover_adresses;
}


bool CoverDownloader::cov_download_cover(QString adress, QImage* img) {


    return WebAccess::read_http_into_img(adress, img);

}


