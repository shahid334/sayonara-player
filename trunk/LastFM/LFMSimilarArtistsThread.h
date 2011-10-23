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

class LFM_SimilarArtists: public QThread {

Q_OBJECT
public:
	LFM_SimilarArtists(QString api_key);
	virtual ~LFM_SimilarArtists();

	void set_artist_name(QString artist_name);
	int get_chosen_id();

protected:
	void run();

private:
	QString _api_key;
	QString _artist_name;
	int		_chosen_id;



};

#endif /* LFMSIMILARARTISTS_H_ */
