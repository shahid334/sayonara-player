/* WebAccess.cpp */

/* Copyright (C) 2012  Lucio Carreras
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


#ifndef WEBACCESS_H_
#define WEBACCESS_H_
#include <QString>
#include <QImage>



namespace WebAccess {

    bool read_http_into_str(QString url, QString* content);
    bool read_http_into_img(QString url, QImage* image);
}


#endif /* WEBACCESS_H_ */
