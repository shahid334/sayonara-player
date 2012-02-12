/*
 * GUI_Stream.h
 *
 *  Created on: Feb 9, 2012
 *      Author: luke
 */

#ifndef GUI_STREAM_H_
#define GUI_STREAM_H_

#include <QDialog>
#include <QMap>
#include <ui_GUI_Stream.h>

class GUI_Stream : public QDialog, private Ui::GUI_stream {

	Q_OBJECT

signals:
	void sig_play_stream(const QString&, const QString&);

public slots:
	void psl_radio_stations_received(const QMap<QString, QString>&);

private slots:
	void listen_clicked();
	void combo_index_changed(int idx);
	void combo_text_changed(const QString& text);
	void delete_clicked();
	void save_clicked();


public:
	GUI_Stream();
	virtual ~GUI_Stream();

private:
	Ui::GUI_stream* ui;

	void init_gui();
	QMap<QString, QString> _stations;
	int _cur_station;
	QString _cur_station_name;
	QString _cur_station_adress;
};

#endif /* GUI_STREAM_H_ */
