/* GUI_InfoDialog.h

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
 * Jul 19, 2012 
 *
 */

#ifndef GUI_INFODIALOG_H_
#define GUI_INFODIALOG_H_

#include "HelperStructs/MetaData.h"
#include "CoverLookup/CoverLookup.h"
#include "StreamPlugins/LastFM/LFMTrackChangedThread.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <ui_GUI_InfoDialog.h>
#include <QWidget>
#include <QString>
#include <QMetaType>



#define INFO_MODE_TRACKS 0
#define INFO_MODE_ARTISTS 1
#define INFO_MODE_ALBUMS 2

class GUI_InfoDialog : public QWidget, private Ui::InfoDialog{
Q_OBJECT

signals:
	void sig_search_cover(const MetaData&);
	void sig_search_artist_image(const QString&);

public slots:
	void psl_image_available(QString);
	void psl_album_info_available(const QString& target_class);
	void psl_artist_info_available(const QString& target_class);
	void psl_corrected_data_available(const QString& target_class);


public:
	GUI_InfoDialog(QWidget* parent =0);
	virtual ~GUI_InfoDialog();

	void setMode(int mode);
	void setMetaData(vector<MetaData>& vd);


private:
	Ui::InfoDialog* ui;
	CoverLookup* _cover_lookup;
	LFMTrackChangedThread* _lfm_thread;
	CDatabaseConnector* _db;
	QString _class_name;


	vector<MetaData> _v_md;

	int _mode;
	int _diff_mode;

	QString _album_name;
	QString _artist_name;

	void prepare_artists();
	void prepare_albums();
	void prepare_tracks();

	void prepare_cover();
	void prepare_lfm_info();
};

#endif /* GUI_INFODIALOG_H_ */
