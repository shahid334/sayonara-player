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

#include "HelperStructs/WebAccess.h"
#include <QString>
#include <QDebug>
#include <curl/curl.h>

#include <string>
#include <stdlib.h>


static QString webpage;


static void wa_free_webpage();
static size_t wa_get_answer( void *ptr, size_t size, size_t nmemb, FILE *userdata);

static
void wa_free_webpage(){

	webpage.clear();

}

static
size_t wa_get_answer( void *ptr, size_t size, size_t nmemb, FILE *userdata){

	(void) userdata;

	char* cptr = (char*) ptr;
	webpage.append(cptr);

	return size * nmemb;
}



bool wa_call_url(const QString& url, QString& response){

	wa_free_webpage();

	CURL *curl = curl_easy_init();

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_URL, url.toLocal8Bit().data());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, wa_get_answer);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 2000);

		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}


	if(webpage.size() > 0){
		qDebug() << "got response from " << url << ":";
		response = webpage;
		wa_free_webpage();
		return true;
	}

	else {
		qDebug() << "got no response from url";
		wa_free_webpage();
		return false;
	}
}

