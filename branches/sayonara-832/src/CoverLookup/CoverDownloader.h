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

namespace CoverDownloader {

    QStringList cov_calc_adresses_from_webpage(uint num, QString& webpage);
    QStringList cov_call_and_parse(QString url, int num_adresses);
    bool cov_download_cover(QString adress, QImage* img);

}

#endif /* COVERDOWNLOADER_H_ */
