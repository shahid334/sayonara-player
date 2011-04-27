/*
 * LibrarySetupWidget.h
 *
 *  Created on: Apr 27, 2011
 *      Author: luke
 */

#ifndef LIBRARYSETUPWIDGET_H_
#define LIBRARYSETUPWIDGET_H_

#include <QDialog>
#include <QObject>
#include <ui_GUI_LibrarySetup_Widget.h>

class LibrarySetupWidget : public QWidget, private Ui::LibrarySetup{

	Q_OBJECT

public:

	LibrarySetupWidget(QWidget* parent=0);
	virtual ~LibrarySetupWidget();

	signals:
		void libpath_changed(QString);

private slots:
		void save_button_clicked(bool b=true);
		void choose_button_clicked(bool b=true);
/*
public slots:
		void show();*/

	private:
		Ui::LibrarySetup* ui;

};

#endif /* LIBRARYSETUPWIDGET_H_ */
