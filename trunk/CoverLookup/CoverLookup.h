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
		void covers_found(vector<QPixmap>&);
		void cover_found(QPixmap&);

	public slots:
		void search_cover(const MetaData& md);
		void search_covers(const vector<Album>&);
		void search_alternative_covers(const QString& album);
		void search_all_covers();
		void terminate_thread();

	private slots:
		void thread_finished();


public:
	CoverLookup();
	virtual ~CoverLookup();



private:
	CoverFetchThread* _thread;
	void download_covers(QStringList adresses, uint num_covers_to_fetch, vector<QPixmap>& vec_pixmaps);

	int _emit_type;

	vector<QPixmap> alternative_covers;
};

#endif /* COVERLOOKUP_H_ */
