/* GUIImportFolder.h */

/* Copyright (C) 2012  Lucio Carreras
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

#include <QDialog>
#include <QStringList>
#include <ui_GUI_ImportFolder.h>


class GUI_ImportFolder : public QDialog, private Ui::ImportFolder {

Q_OBJECT
public:
    GUI_ImportFolder(QWidget* parent, const QStringList&, bool copy_enabled);
	virtual ~GUI_ImportFolder();

	signals:
		void accepted(const QString&, bool);
		void rejected();




	private slots:
		void bb_accepted();
		void bb_recjected();
		void combo_box_changed(const QString&);

	public:
		void progress_changed(int);


private:
	Ui::ImportFolder* ui;

};

#endif /* GUIIMPORTFOLDER_H_ */
