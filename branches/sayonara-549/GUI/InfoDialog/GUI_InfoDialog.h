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
#include "LyricLookup/LyricLookup.h"
#include "StreamPlugins/LastFM/LFMTrackChangedThread.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "GUI/tagedit/GUI_TagEdit.h"
#include "GUI/alternate_covers/GUI_Alternate_Covers.h"
#include "GUI/ui_GUI_InfoDialog.h"

#include <QWidget>
#include <QDialog>
#include <QString>
#include <QMetaType>
#include <QCloseEvent>



#define INFO_MODE_TRACKS 0
#define INFO_MODE_ARTISTS 1
#define INFO_MODE_ALBUMS 2

#define TAB_INFO 0
#define TAB_LYRICS 1
#define TAB_EDIT 2

class GUI_InfoDialog : public QDialog, private Ui::InfoDialog{
Q_OBJECT


public slots:
	void psl_image_available(QString, QString);
	void psl_album_info_available(const QString& target_class);
	void psl_artist_info_available(const QString& target_class);
	void psl_corrected_data_available(const QString& target_class);
    void changeSkin(bool dark);

private slots:
	void psl_lyrics_available();
	void psl_lyrics_server_changed(int);
	void psl_id3_success(bool);
    void alternate_covers_available(QString, QString);
    void no_cover_available();
	void cover_clicked();


protected:
    void closeEvent(QCloseEvent *e);

public:
	GUI_InfoDialog(QWidget* parent, GUI_TagEdit* tag_edit=0);
	virtual ~GUI_InfoDialog();

	void setMode(int mode);
	void setMetaData(const MetaDataList& vd);
    void set_tag_edit_visible(bool b);

	void show(int tab);


private:
	Ui::InfoDialog* 		ui;
	GUI_TagEdit*			ui_tag_edit;

	GUI_Alternate_Covers*	_alternate_covers;
	CoverLookup* 			_cover_lookup;
	LFMTrackChangedThread* 	_lfm_thread;
	LyricLookupThread*		_lyric_thread;
	CDatabaseConnector* 	_db;
	QString 				_class_name;
	int						_lyric_server;
	bool					_lyrics_visible;
    bool                    _initialized;
    bool                    _tag_edit_visible;
    bool                    _dark;

	MetaDataList _v_md;

	int _mode;
	int _diff_mode;

	QString _album_name;
	QString _artist_name;
	QString	_title;

	void prepare_artists();
	void prepare_albums();
	void prepare_tracks();

	void prepare_cover();
	void prepare_lfm_info();
	void prepare_lyrics();

    void init();


};

#endif /* GUI_INFODIALOG_H_ */
