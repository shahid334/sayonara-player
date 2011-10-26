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
#include <QList>

LFM_SimilarArtists::LFM_SimilarArtists(QString api_key) {
	_api_key = api_key;

}

LFM_SimilarArtists::~LFM_SimilarArtists() {
	// TODO Auto-generated destructor stub
}

QMap<QString, int> LFM_SimilarArtists::filter_available_artists(QMap<QString, double> *artists, int idx){

		int start_idx = idx;

		QMap<QString, int> possible_artists;
		QStringList possible_artist_names;

		while(possible_artists.size() == 0){

			foreach(QString key, artists[idx].keys()){

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

		return possible_artists;
}

void LFM_SimilarArtists::run(){


	 srand ( time(NULL) );

		QString url = 	QString("http://ws.audioscrobbler.com/2.0/?");
		QString encoded = QUrl::toPercentEncoding(_artist_name);
		url += QString("method=artist.getsimilar&");
		url += QString("artist=") + encoded + QString("&");
		url += QString("api_key=") + _api_key;

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

		doc.clear();

		qDebug() << "Hier";


		// get random list where to search the artist in
		int idx = 0;
		int rnd = rand() % 1000;
		if(rnd > 600) idx = 0;			// [500-999]
		else if(rnd > 250) idx = 1;		// [200-500]
		else idx = 2;

		qDebug() << "Hier 2";

		QMap<QString, int> possible_artists =
				filter_available_artists(artist_match, idx);

		if(possible_artists.size() == 0) return;

		//QString random_artist = possible_artist_names[rand() % possible_artist_names.size()];



		for(QMap<QString, int>::iterator it = possible_artists.begin(); it != possible_artists.end(); it++){
			qDebug() << "Push back " << it.value();
			_chosen_ids.push_back(it.value());
		}
}


void LFM_SimilarArtists::set_artist_name(QString artist_name){
	_artist_name = artist_name;
}

QList<int> LFM_SimilarArtists::get_chosen_ids(){
	return _chosen_ids;
}
