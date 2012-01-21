/*
 * GUIPlaylistChooser.h
 *
 *  Created on: Jan 2, 2012
 *      Author: luke
 */

#ifndef GUIPLAYLISTCHOOSER_H_
#define GUIPLAYLISTCHOOSER_H_

#include "HelperStructs/MetaData.h"

#include <vector>
#include <QMap>
#include <QWidget>
#include <QDockWidget>
#include <ui_GUI_PlaylistChooser.h>

using namespace std;

class GUI_PlaylistChooser : public QDockWidget, private Ui::PlaylistChooser {

Q_OBJECT


signals:
	void sig_playlist_chosen(int);
	void sig_delete_playlist(int);
	void sig_save_playlist(int);
	void sig_save_playlist(QString);
	void sig_closed();
	void sig_clear_playlist();


private slots:
	void apply_button_pressed();
	void save_button_pressed();
	void save_as_button_pressed();
	void delete_button_pressed();
	void playlist_selected(int);
	void set_radio_active(bool);


public slots:
	void all_playlists_fetched(QMap<int, QString>&);

public:
	GUI_PlaylistChooser(QWidget* parent=0);
	virtual ~GUI_PlaylistChooser();

private:
	Ui::PlaylistChooser* ui;
	QMap<int, QString> _playlists;
	int	_cur_idx;
	bool _started;

protected:
	void closeEvent(QCloseEvent* e);
};

#endif /* GUIPLAYLISTCHOOSER_H_ */
