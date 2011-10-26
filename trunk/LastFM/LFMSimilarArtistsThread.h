/*
 * LFMSimilarArtists.h
 *
 *  Created on: Oct 22, 2011
 *      Author: luke
 */

#ifndef LFMSIMILARARTISTS_H_
#define LFMSIMILARARTISTS_H_

#include <QThread>
#include <QString>
#include <QList>
#include <QMap>


class LFM_SimilarArtists: public QThread {

Q_OBJECT
public:
	LFM_SimilarArtists(QString api_key);
	virtual ~LFM_SimilarArtists();

	void set_artist_name(QString artist_name);
	QList<int> get_chosen_ids();

protected:
	void run();

private:
	QString 		_api_key;
	QString 		_artist_name;
	QList<int>		_chosen_ids;

	QMap<QString, int> filter_available_artists(QMap<QString, double> *artists, int idx);



};

#endif /* LFMSIMILARARTISTS_H_ */
