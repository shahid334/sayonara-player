/* LFMWebAccess.h */

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
 * LFMWebAccess.h
 *
 *  Created on: Oct 22, 2011
 *      Author: luke
 */

#ifndef LFMWEBACCESS_H_
#define LFMWEBACCESS_H_

#include <QString>
#include <qdom.h>
#include <curl/curl.h>
#include <string>
#include <stdlib.h>
#include <QDebug>

using namespace std;


void lfm_wa_init();

void lfm_wa_free_webpage();

const char* lfm_wa_get_url_enc(QString str);

size_t lfm_wa_get_answer( void *ptr, size_t size, size_t nmemb, FILE *userdata);


bool lfm_wa_call_session_url(QString url);


bool lfm_wa_call_scrobble_url(string url, string post_data);


bool lfm_wa_call_lfm_url(QString url, QDomDocument& doc);

QString lfm_wa_parse_session_answer();




#endif /* LFMWEBACCESS_H_ */
