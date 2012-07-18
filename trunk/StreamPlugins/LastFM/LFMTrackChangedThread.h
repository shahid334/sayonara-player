/* LFMTrackChangedThread.h

 * Copyright (C) 2012  
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras, 
 * Jul 18, 2012 
 *
 */

#ifndef LFMTRACKCHANGEDTHREAD_H_
#define LFMTRACKCHANGEDTHREAD_H_

#include <QThread>
#include <QList>
#include <QMap>
#include <QString>
#include "StreamPlugins/LastFM/LFMGlobals.h"

#include "HelperStructs/MetaData.h"

class LFMTrackChangedThread : public QThread{

	Q_OBJECT

	signals:
		void sig_corrected_data_available();
		void sig_similar_artists_available(const QList<int>&);

protected:
		void run();

public:
	LFMTrackChangedThread(QString api_key, QString username, QString session_key);
	virtual ~LFMTrackChangedThread();

	void setTrackInfo(const MetaData& md);
	bool getCorrections(MetaData& md, bool& loved, bool& corrected);


private:
	MetaData 	_md;
	QString		_username;
	QString 	_api_key;
	QString 	_session_key;
	QList<int>	_chosen_ids;

	MetaData	_md_corrected;
	bool		_loved;
	bool		_corrected;
	bool		_corrected_success;


	bool search_similar_artists();
	bool update_now_playing();
	bool get_corrected_track_info(MetaData& md, bool& loved, bool& corrected);

	QMap<QString, int> filter_available_artists(QMap<QString, double> *artists, int idx);

};

#endif /* LFMTRACKCHANGEDTHREAD_H_ */
