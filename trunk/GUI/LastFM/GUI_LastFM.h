/*
 * GUI_LastFM.h
 *
 *  Created on: Apr 21, 2011
 *      Author: luke
 */

#ifndef GUI_LASTFM_H_
#define GUI_LASTFM_H_

#include <QObject>
#include <QString>
#include <ui_GUI_LastFM_Widget.h>

class GUI_LastFM : public QWidget, private Ui_GUI_LastFM_Widget{

	Q_OBJECT

public:
	GUI_LastFM();
	GUI_LastFM(QString username, QString password);

	virtual ~GUI_LastFM();

	signals:
		void new_lfm_credentials(QString, QString);

	private slots:
		void save_button_pressed();


	public slots:
		void show_win();

	private:
		Ui_GUI_LastFM_Widget* ui;


	public:
		void set_username_and_password(QString username, QString password);


};

#endif /* GUI_LASTFM_H_ */
