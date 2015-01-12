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

#include "HelperStructs/SayonaraClass.h"
#include "HelperStructs/MetaData.h"
#include "CoverLookup/CoverLookup.h"
#include "LyricLookup/LyricLookup.h"
#include "StreamPlugins/LastFM/LFMTrackChangedThread.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "GUI/tagedit/GUI_TagEdit.h"
#include "GUI/alternate_covers/GUI_Alternate_Covers.h"
#include "GUI/ui_GUI_InfoDialog.h"

#include <QMetaType>
#include <QCloseEvent>


#define INFO_MODE_TRACKS 0
#define INFO_MODE_ARTISTS 1
#define INFO_MODE_ALBUMS 2

#define TAB_INFO 0
#define TAB_LYRICS 1
#define TAB_EDIT 2

enum InfoDialogMode {
	InfoDialogMode_Tracks,
	InfoDialogMode_Albums,
	InfoDialogMode_Artists
};

class GUI_InfoDialog : public SayonaraDialog, private Ui::InfoDialog{
Q_OBJECT

signals:
	void sig_cover_changed(const CoverLocation&);

public slots:

    void changeSkin(bool dark);
    void language_changed();
	void set_metadata(const MetaDataList& vd);
	void show(int tab);

private slots:
	void psl_lyrics_available();
	void psl_lyrics_server_changed(int);
	void psl_tab_index_changed(int);
    void no_cover_available();
	void cover_clicked();
	void psl_cover_available(const CoverLocation&);

    void psl_tag_edit_deleted();


protected:
    void closeEvent(QCloseEvent *e);

public:
    GUI_InfoDialog(QWidget* parent, GUI_TagEdit* tag_edit);
	virtual ~GUI_InfoDialog();

	void setInfoMode(InfoDialogMode mode);


    void set_tag_edit_visible(bool b);


private:

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
    QString                 _call_id;
	InfoDialogMode			_mode;

    QString 				_cover_artist;
	QString					_cover_album;
	CoverLocation			_cl;

	MetaDataList _v_md;

	void prepare_cover(const CoverLocation& cover_path);
	void prepare_lyrics();
	void prepare_info();

    void init();


};

#endif /* GUI_INFODIALOG_H_ */
