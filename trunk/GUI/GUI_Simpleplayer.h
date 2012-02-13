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
#include "GUI/playlist_chooser/GUI_PlaylistChooser.h"
#include "GUI/radio/GUI_RadioWidget.h"
#include "GUI/stream/GUI_Stream.h"

#include <QMainWindow>
#include <QCloseEvent>
#include <GUI_TrayIcon.h>


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
    void cover_changed(bool, QString);
    void fillSimplePlayer (const MetaData & in);
    void showEqualizer(bool b=false);
    void showRadio(bool b=false);
    void showPlaylistChooser(bool b=false);

    /**
      * Set current position in filestream
      */
    void setCurrentPosition (quint32 pos_sec);
    void update_info(const MetaData&);
    void close_eq();
    void close_playlist_chooser();
    void close_radio();

    void set_radio_active(int);




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
    void sig_stream_selected(const QString&, const QString&);
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
    void importDirectory(QString);
    void libpath_changed(QString);
    void fetch_all_covers();
    void show_playlists();



private slots:
    void playClicked(bool b = true);
    void stopClicked(bool b = true);
    void backwardClicked(bool b = true);
    void forwardClicked(bool b = true);
    void fileSelectedClicked(bool);
    void streamDialogClicked(bool);
    void play_stream_selected(const QString&, const QString& );
    void folderSelectedClicked(bool);
    void total_time_changed(qint64);
    void searchSliderPressed();
    void searchSliderReleased();
    void searchSliderMoved(int search_percent, bool by_app=false);

    void coverClicked(bool);
    void muteButtonPressed();
    void album_cover_pressed();

    void trayItemActivated (QSystemTrayIcon::ActivationReason reason);

    void showLibrary(bool);

    void changeSkin(bool);
    void lastFMClicked(bool b = true);
    void reloadLibraryClicked(bool b = true);
    void importFolderClicked(bool b = true);

    void setLibraryPathClicked(bool = true);
    void fetch_all_covers_clicked(bool b = true);
    void load_pl_on_startup_toggled(bool);

    void volumeChangedSlider(int volume_percent);
    void volumeChangedByTick(int val);

    void show_notification_toggled(bool);


    void about(bool b=false);





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
    GUI_PlaylistChooser*		ui_playlist_chooser;
    GUI_RadioWidget*			ui_radio;
    GUI_Stream*					ui_stream_dialog;
    quint32 					m_completeLength_ms;
    bool 						m_playing;
    bool 						m_cur_searching;
    bool						m_mute;

    GUI_TrayIcon *				m_trayIcon;

    /// some shared actions
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

    bool						m_isEqHidden;
    bool						m_isPcHidden;
    bool						m_isRadioHidden;

    int							m_radio_active;

    const quint8				VOLUME_STEP_SIZE_PERC;
    int 						m_library_width;
    int							m_library_stretch_factor;



    void setupActions ();
    void connectTrayIcon();

    void setupVolButton(int percent);
    void setupIcons();
    void initGUI();


public:
    void setPlaylist(GUI_Playlist* playlist);
    void setLibrary(GUI_Library_windowed* library);
    void setEqualizer(GUI_Equalizer* eq);
    void setPlaylistChooser(GUI_PlaylistChooser* playlist_chooser);
    void setRadio(GUI_RadioWidget* radio);

    void hideAllPlugins();


    QWidget* getParentOfPlaylist();
    QWidget* getParentOfPlaylistChooser();
    QWidget* getParentOfLibrary();
    QWidget* getParentOfEqualizer();
    QWidget* getParentOfRadio();

    void setVolume(int vol);
    void setStyle(int);

};

#endif // GUI_SIMPLEPLAYER_H
