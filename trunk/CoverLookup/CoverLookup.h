/*
 * CoverLookup.h
 *
 *  Created on: Apr 4, 2011
 *      Author: luke
 */

#ifndef COVERLOOKUP_H_
#define COVERLOOKUP_H_

#include "HelperStructs/MetaData.h"
#include "CoverLookup/CoverFetchThread.h"

#include <string>
#include <vector>

#include <QObject>
#include <QThread>
#include <QPixmap>
#include <QCryptographicHash>
#include <QDir>

#define EMIT_NONE 0
#define EMIT_ONE 1
#define EMIT_ALL 2


using namespace std;


class CoverLookup : public QObject{

	Q_OBJECT

	signals:
		void covers_found(vector<QPixmap>&);		/* emit if multiple covers are found (player) */
		void cover_found(QPixmap&);					/* emit if single cover is found (player) */
		void new_cover_found(const QPixmap&);		/* emit if a new cover is found (alternate covers) */

	public slots:
		void search_cover(const MetaData& md);		/* search a cover for certain metadata */
		void search_covers(const vector<Album>&);	/* search multiple covers */
		void search_alternative_covers(const QString&);		/* search alternative covers for one album */
		void search_alternative_covers(const MetaData&);	/* search alternative covers for one album */
		void search_all_covers();					/* search all covers*/
		void terminate_thread();					/* stop to search for covers */

	private slots:
		void thread_finished();


public:

	virtual ~CoverLookup();

	static CoverLookup* getInstance();

	bool get_found_cover(int idx, QPixmap& p);

protected:
	CoverLookup();

private:
	CoverLookup(const CoverLookup&);

	CoverFetchThread* _thread;
	void download_covers(QStringList adresses, uint num_covers_to_fetch, vector<QPixmap>& vec_pixmaps);
	Album get_album_from_metadata(const MetaData& md);

	int _emit_type;

	vector<QPixmap> _alternative_covers;
};

#endif /* COVERLOOKUP_H_ */
