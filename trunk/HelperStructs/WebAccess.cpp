/*
 * WebAccess.cpp
 *
 *  Created on: Feb 12, 2012
 *      Author: luke
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

