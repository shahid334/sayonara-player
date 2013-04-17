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
#include <QImage>
#include <QDebug>
#include <curl/curl.h>

#include <string>
#include <stdlib.h>

#define DOWNLOAD_INCOMPLETE 0
#define DOWNLOAD_COMPLETE 1

int download_status;

static QString webpage;
static int webpage_bytes;;

static void wa_free_webpage();
static size_t wa_get_answer( void *ptr, size_t size, size_t nmemb, FILE *userdata);
static bool wa_call_url(const QString& url, QString& response);
static int wa_progress(void *p, double dltotal, double dlnow, double ultotal, double ulnow);


static int wa_progress(void *p,
                    double dltotal, double dlnow,
                    double ultotal, double ulnow)
{

  (void) p;

 // qDebug() << dlnow << "/" << dltotal;

  if(dlnow >= dltotal) download_status = DOWNLOAD_COMPLETE;
  else download_status = DOWNLOAD_INCOMPLETE;

  return 0;
}


static
void wa_free_webpage(){

	webpage.clear();
	webpage_bytes = 0;

}

static
size_t wa_get_answer( void *ptr, size_t size, size_t nmemb, FILE *userdata){

	(void) userdata;

	char* cptr = (char*) ptr;
	webpage.append(QString::fromLatin1(cptr, size*nmemb));

	webpage_bytes += (size * nmemb);
	return size * nmemb;
}



bool wa_call_url(const QString& url, QString& response){

	wa_free_webpage();

	download_status = DOWNLOAD_INCOMPLETE;
	CURL *curl = curl_easy_init();

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_URL, url.toLocal8Bit().data());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, wa_get_answer);
		//curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 2500);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, wa_progress);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}


	if(webpage_bytes > 0){

		response = webpage;
		if(download_status == DOWNLOAD_COMPLETE)
			return true;
		else return false;
	}

	else {
		qDebug() << "got no response from url " << url;
		return false;
	}
}


bool WebAccess::read_http_into_str(QString url, QString& content){

	content.clear();
    wa_call_url(url, content);

    if(content.size() > 0)	return true;
    else{
        qDebug() << "Read Http into str:: url: " << url;
        qDebug() << "Read Http into str:: could not get any content";
    }

    return false;
}

bool WebAccess::read_http_into_img(QString url, QImage& img){

    QString content;
    if( !wa_call_url(url, content) ) return false;



    return img.loadFromData(content.toAscii());
}


