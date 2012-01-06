/*
 * GUIImportFolder.h
 *
 *  Created on: Dec 7, 2011
 *      Author: luke
 */

#ifndef GUIIMPORTFOLDER_H_
#define GUIIMPORTFOLDER_H_

#include <QWidget>
#include <QStringList>
#include <ui_GUI_ImportFolder.h>


class GUI_ImportFolder : public QWidget, private Ui::ImportFolder {

Q_OBJECT
public:
	GUI_ImportFolder(QWidget* widget, const QStringList&, bool copy_enabled);
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
