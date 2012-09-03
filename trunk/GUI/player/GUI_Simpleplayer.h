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
#include "GUI/LFMRadio/GUI_LFMRadioWidget.h"
#include "GUI/stream/GUI_Stream.h"
#include "Engine/Engine.h"
#include "CoverLookup/CoverLookup.h"

#include <QMainWindow>
#include <QCloseEvent>
#include "GUI/player/GUI_TrayIcon.h"


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
    void cover_changed(QString, QString);
    void update_track (const MetaData & in);
    void show_eq(bool b=false);
    void show_stream(bool b=false);
    void show_lfm_radio(bool b=false);
    void show_playlist_chooser(bool b=false);
    void showPlugin(QWidget* widget, bool v);
    void hideUnneededPlugins(QWidget* wannashow);
    void lfm_info_fetched(const MetaData& md, bool loved, bool corrected);

    /**
      * Set current position in filestream
      */
    void setCurrentPosition (quint32 pos_sec);
    void psl_id3_tags_changed(vector<MetaData>& v_md);
    void close_eq();
    void close_playlist_chooser();
    void close_stream();
    void close_lfm_radio();

    void set_radio_active(int);
    void psl_strrip_set_active(bool);




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
    void sig_rec_button_toggled(bool);
    void fileSelected (QStringList & filelist);
    void sig_stream_selected(const QString&, const QString&);
    void baseDirSelected (const QString & baseDir);
    void search(int pos_percent);
    void sig_volume_changed (int);
    void sig_want_cover(const MetaData &);
    void sig_fetch_alternate_covers(const MetaData&);
    void sig_want_more_covers();
    void sig_fetch_all_covers();
    void playlistCreated(QStringList&);
    void skinChanged(bool);
    void windowResized(const QSize&);
    void setupLastFM();
    void reloadLibrary();
    void importDirectory(QString);
    void libpath_changed(QString);

    void show_playlists();
    void show_small_playlist_items(bool);
    void sig_sound_engine_changed(QString&);
    void sig_correct_id3(const MetaData&);
    void sig_show_stream_rec(bool);

    void sig_show_socket();

private slots:
    void playClicked(bool b = true);
    void stopClicked(bool b = true);
    void backwardClicked(bool b = true);
    void forwardClicked(bool b = true);
    void sl_rec_button_toggled(bool b);
    void fileSelectedClicked(bool);

    void folderSelectedClicked(bool);
    void total_time_changed(qint64);
    void searchSliderPressed();
    void searchSliderReleased();
    void searchSliderMoved(int search_percent, bool by_app=false);
    void correct_btn_clicked(bool b=false);

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
    void min2tray_toggled(bool);
    void small_playlist_items_toggled(bool);

    void sound_engine_phonon_clicked();
    void sound_engine_gst_clicked();
    void populate_engines(const QList<Engine*>& lists, int active);
    void sl_action_streamripper_toggled(bool);
    void sl_action_socket_connection_triggered(bool);



    void about(bool b=false);


protected:


    void changeEvent(QEvent *event);
    void keyPressEvent(QKeyEvent* e);
    void resizeEvent(QResizeEvent* e);




private:


    Ui::GUI_SimplePlayer*		ui;


    GUI_Playlist* 				ui_playlist;
    GUI_Library_windowed*		ui_library;
    GUI_Equalizer*				ui_eq;
    GUI_PlaylistChooser*		ui_playlist_chooser;
    GUI_Stream*					ui_stream;
    GUI_LFMRadioWidget*			ui_lfm_radio;
    CoverLookup*				m_cov_lookup;



    QString						m_class_name;
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

    QString						m_skinSuffix;
    QString						m_album;
    QString						m_artist;
    MetaData					m_metadata;
    MetaData					m_metadata_corrected;
    bool						m_min2tray;


    int							m_radio_active;

    const quint8				VOLUME_STEP_SIZE_PERC;
    int 						m_library_width;
    int							m_library_stretch_factor;


    QString getLengthString (quint32 length_ms) const;


    void setupTrayActions ();

    void setupVolButton(int percent);
    void setupIcons();
    void initGUI();
    void setupConnections();



public:
    void setPlaylist(GUI_Playlist* playlist);
    void setLibrary(GUI_Library_windowed* library);
    void setEqualizer(GUI_Equalizer* eq);
    void setPlaylistChooser(GUI_PlaylistChooser* playlist_chooser);
    void setStream(GUI_Stream* stream);
    void setLFMRadio(GUI_LFMRadioWidget* lfm_radio);

    void hideAllPlugins();
    void check_show_plugins();


    QWidget* getParentOfPlaylist();
    QWidget* getParentOfLibrary();
    QWidget* getParentOfPlugin();

    void setVolume(int vol);
    void setStyle(int);

};

#endif // GUI_SIMPLEPLAYER_H
