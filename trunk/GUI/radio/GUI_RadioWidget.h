/*
 * GUIRadioWidget.h
 *
 *  Created on: Oct 22, 2011
 *      Author: luke
 */

#ifndef GUIRADIOWIDGET_H_
#define GUIRADIOWIDGET_H_

#include <ui_GUI_RadioWidget.h>
#include <QDockWidget>
#include <QWidget>

class GUI_RadioWidget : public QDockWidget, private Ui::RadioWidget{

Q_OBJECT

public:
	GUI_RadioWidget(QWidget* parent=0);
	virtual ~GUI_RadioWidget();


	signals:
		void listen_clicked(const QString&, bool artist);

	private slots:
		void start_listen();
		void radio_button_changed();



	private:

		Ui::RadioWidget* _ui;

};

#endif /* GUIRADIOWIDGET_H_ */
