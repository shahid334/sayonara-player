/*
 * GUI_PlaylistEntry.h
 *
 *  Created on: Apr 9, 2011
 *      Author: luke
 */

#ifndef GUI_PLAYLISTENTRY_H_
#define GUI_PLAYLISTENTRY_H_

#include "ui_GUI_Playlist_Entry.h"

#include <QObject>


class GUI_PlaylistEntry : public QFrame, private Ui::PlaylistEntry {

	Q_OBJECT
public:
	GUI_PlaylistEntry(QWidget* parent=0);
	virtual ~GUI_PlaylistEntry();

public:
	void setArtist(QString artist);
	void setAlbum(QString album);
	void setTitle(QString title);
	void setTime(QString time);

	void setBackgroundColorPlaying();
	void setBackgroundColorNotPlaying();

	void setWidth(int w);


private:
	Ui::PlaylistEntry* ui;

};

#endif /* GUI_PLAYLISTENTRY_H_ */
