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
