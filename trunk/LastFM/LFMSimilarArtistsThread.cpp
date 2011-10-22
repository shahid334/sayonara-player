/*
 * LFMSimilarArtists.cpp
 *
 *  Created on: Oct 22, 2011
 *      Author: luke
 */

#include "LastFM/LFMSimilarArtistsThread.h"
#include "LastFM/LFMWebAccess.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include <QMap>
#include <QString>
#include <QStringList>
#include <qdom.h>
#include <QUrl>

LFM_SimilarArtists::LFM_SimilarArtists(QString api_key) {
	_api_key = api_key;

}

LFM_SimilarArtists::~LFM_SimilarArtists() {
	// TODO Auto-generated destructor stub
}

/*
bool call_lfm_url(QString url, QDomDocument& doc){

	CURL *curl = curl_easy_init();

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_URL, url.toUtf8().data());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_answer);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 5000);

		curl_easy_perform(curl);

	}

	long int t = 5000000;
	while(lfm_webpage_bytes == 0){
		usleep(100000);
		t -= 100000;
		if( t <= 0) break;
	}


	lfm_webpage = (char*) (realloc(lfm_webpage, lfm_webpage_bytes + 1));
	lfm_webpage[lfm_webpage_bytes] = '\0';

	if(lfm_webpage_bytes > 0){

		QString xmlString = QString(lfm_webpage);
		doc.setContent(xmlString, false);

		free_webpage();

		curl_easy_cleanup(curl);

		return true;

	}

	else {
		free_webpage();
		qDebug() << "webpage is null";
		curl_easy_cleanup(curl);
		return false;
	}

}

*/

void LFM_SimilarArtists::run(){

	_chosen_id = -1;
	 srand ( time(NULL) );

		QString url = 	QString("http://ws.audioscrobbler.com/2.0/?");
		QString encoded = QUrl::toPercentEncoding(_artist_name);
		url += QString("method=artist.getsimilar&");
		url += QString("artist=") + encoded + QString("&");
		url += QString("api_key=") + _api_key;

		qDebug() << url;
		QDomDocument doc("similar_artists");
		lfm_wa_call_lfm_url(url, doc);

		QDomElement docElement = doc.documentElement();
		QDomNode similarartists = docElement.firstChild();			// similarartists

		// 0= very good match
		// 1= good match
		// 2= ok match
		QMap<QString, double> artist_match[3];

		if(similarartists.hasChildNodes()){

			QString artist_name = "";
			double match = -1.0;

			for(int idx_artist=0; idx_artist < similarartists.childNodes().size(); idx_artist++){
				QDomNode artist = similarartists.childNodes().item(idx_artist);

				if(artist.nodeName().toLower().compare("artist") != 0) continue;

				if(!artist.hasChildNodes()) continue;

				for(int idx_content = 0; idx_content <artist.childNodes().size(); idx_content++){
					QDomNode content = artist.childNodes().item(idx_content);
					if(content.nodeName().toLower().contains("name")){
						QDomElement e = content.toElement();
						if(!e.isNull()){
							artist_name = e.text();
						}
					}

					if(content.nodeName().toLower().contains("match")){
						QDomElement e = content.toElement();
						if(!e.isNull()){
							match = e.text().toDouble();
						}
					}

					if(artist_name.size() > 0 && match >= 0) {
						if(match > 0.7){
							artist_match[0][artist_name] = match;
						}
						else if(match > 0.3){
							artist_match[1][artist_name] = match;
						}

						else {
							artist_match[2][artist_name] = match;
						}
						artist_name = "";
						match = -1.0;
						break;
					}
				}
			}
		}

		int idx = 0;
		int rnd = rand() % 1000;
		if(rnd > 600) idx = 0;			// [500-999]
		else if(rnd > 250) idx = 1;		// [200-500]
		else idx = 2;

		int start_idx = idx;


		QMap<QString, int> possible_artists;
		QStringList possible_artist_names;

		while(possible_artists.size() == 0){

			foreach(QString key, artist_match[idx].keys()){

				int artist_id = CDatabaseConnector::getInstance()->getArtistID(key);
				if(artist_id != -1){
					possible_artists[key] = artist_id;
					possible_artist_names.push_back(key);

				}
			}

			if(possible_artists.size() == 0) {
				idx = (idx + 1) % 3;
				if(start_idx == idx) break;
			}
			else break;
		}

		if(possible_artists.size() == 0) return;

		QString random_artist = possible_artist_names[rand() % possible_artist_names.size()];


		_chosen_id = possible_artists.value(random_artist);
		qDebug() << "Chosen artist = " << random_artist << ", " << _chosen_id;

}


void LFM_SimilarArtists::set_artist_name(QString artist_name){
	_artist_name = artist_name;
}

int LFM_SimilarArtists::get_chosen_id(){
	return _chosen_id;
}
