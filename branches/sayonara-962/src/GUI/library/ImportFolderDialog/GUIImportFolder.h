/* GUIImportFolder.h */

/* Copyright (C) 2011 - 2014  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GUIIMPORTFOLDER_H_
#define GUIIMPORTFOLDER_H_


#include <QCloseEvent>
#include <QShowEvent>
#include <QStringList>
#include "GUI/tagedit/GUI_TagEdit.h"
#include "TagEdit/TagEdit.h"
#include "GUI/ui_GUI_ImportFolder.h"
#include "HelperStructs/SayonaraClass.h"

#define IMPORT_DIALOG_CANCELLED -3
#define IMPORT_DIALOG_ROLLBACK -2
#define IMPORT_DIALOG_CACHING -1
#define IMPORT_DIALOG_NO_TRACKS 0

class GUI_ImportFolder : public SayonaraDialog, private Ui::ImportFolder {

Q_OBJECT
public:
	GUI_ImportFolder(QWidget* parent, TagEdit* tag_edit, bool copy_enabled);
	virtual ~GUI_ImportFolder();
    void set_progress(int);
	void set_status(int n_tracks);
	void set_thread_active(bool);
    void set_folderlist(const QStringList& lst);
	void show_info(const QString& str);
	void show_warning(const QString& str);


signals:
    void sig_accepted(const QString&, bool);
    void sig_cancelled();
	void sig_opened();
	void sig_closed();


private slots:
	void bb_accepted();
    void bb_rejected();
	void combo_box_changed(const QString&);
    void choose_dir();


public slots:
    void changeSkin(bool);

protected:
	void closeEvent(QCloseEvent* e);
	void showEvent(QShowEvent* e);


private:

	bool			_thread_active;
	GUI_TagEdit*	_ui_tag_edit;

};

#endif /* GUIIMPORTFOLDER_H_ */
