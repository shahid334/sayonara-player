/* GUI_Simpleplayer.h */

/* Copyright (C) 2011  Lucio Carreras
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


#ifndef GUI_SIMPLEPLAYER_H
#define GUI_SIMPLEPLAYER_H

#include "HelperStructs/MetaData.h"
#include "GUI/playlist/GUI_Playlist.h"
#include "GUI/library/GUI_Library_windowed.h"
#include "GUI/equalizer/GUI_Equalizer.h"
//#include "GUI/radio/GUI_RadioWidget.h"

#include <QMainWindow>
#include <QCloseEvent>
#include <QSystemTrayIcon>

#include <string>

class QCloseEvent;



namespace Ui {
    class GUI_SimplePlayer;
}


class GUI_SimplePlayer : public QMainWindow
{
    Q_OBJECT
public:
    explicit GUI_SimplePlayer(QWidget *parent = 0);
    ~GUI_SimplePlayer();

public slots:
    /**
      * Insert Meta informations
      */
	void cover_changed(bool);
    void fillSimplePlayer (const MetaData & in);
    void showEqualizer(bool b=false);
    void showRadio(bool b=false);

    /**
      * Set current position in filestream
      */
    void setCurrentPosition (quint32 pos_sec);
    void update_info(const MetaData&);
    void close_eq();





signals:
    /**
      * Signals emitted after pressing buttons
      */
    void play();
    void pause();
    void stop();
    void backward();
    void forward();
    void mute();
    void fileSelected (QStringList & filelist);
    void baseDirSelected (const QString & baseDir);
    void search(int pos_percent);
    void volumeChanged (qreal vol_percent);
    void wantCover(const MetaData &);
    void fetch_alternate_covers(const MetaData&);
    void wantMoreCovers();
    void playlistCreated(QStringList&);
    void skinChanged(bool);
    void windowResized(const QSize&);
    void setupLastFM();
    void reloadLibrary();
    void libpath_changed(QString);
    void fetch_all_covers();



private slots:
    void playClicked(bool b = true);
    void stopClicked(bool b = true);
    void backwardClicked(bool b = true);
    void forwardClicked(bool b = true);
    void fileSelectedClicked(bool);
    void folderSelectedClicked(bool);
    void total_time_changed(qint64);
    void searchSliderPressed();
    void searchSliderReleased();
    void searchSliderMoved(int search_percent, bool by_app=false);
    void volumeChangedSlider(int volume_percent);
    void coverClicked(bool);
    void showAgain(QSystemTrayIcon::ActivationReason);
    void muteButtonPressed();
    void album_cover_pressed();

    void changeSkin(bool);
    void lastFMClicked(bool = true);
    void reloadLibraryClicked(bool = true);

    void setLibraryPathClicked(bool = true);
    void fetch_all_covers_clicked(bool b = true);
    void load_pl_on_startup_toggled(bool);

protected:


    void changeEvent(QEvent *event);
    void keyPressEvent(QKeyEvent* e);
    void resizeEvent(QResizeEvent* e);




private:
    QString getLengthString (quint32 length_ms) const;

    Ui::GUI_SimplePlayer*		ui;
    GUI_Playlist* 				ui_playlist;
    GUI_Library_windowed*		ui_library;
    GUI_Equalizer*				ui_eq;
//    GUI_RadioWidget*			ui_radio;
    quint32 					m_completeLength_ms;
    bool 						m_playing;
    bool 						m_cur_searching;
    bool						m_mute;

    QSystemTrayIcon*			m_trayIcon;

    QAction*					m_closeAction;
   	QAction*					m_playAction;
   	QAction*					m_stopAction;
   	QAction*					m_muteAction;
   	QAction*					m_fwdAction;
   	QAction*					m_bwdAction;


   	bool						m_minimized2tray;
    bool						m_minTriggerByTray;


    QString						m_skinSuffix;
    QString						m_album;
    QString						m_artist;
    MetaData					m_metadata;




    void setupTrayContextMenu();
    void setupVolButton(int percent);
	void setupIcons();
	void initGUI();

public:
    void setPlaylist(GUI_Playlist* playlist);
    void setLibrary(GUI_Library_windowed* library);
    void setEqualizer(GUI_Equalizer* eq);
//    void setRadio(GUI_RadioWidget* radio);
    QWidget* getParentOfPlaylist();
    QWidget* getParentOfLibrary();
    QWidget* getParentOfEqualizer();
    void setVolume(int vol);
    void setStyle(int);

};

#endif // GUI_SIMPLEPLAYER_H
