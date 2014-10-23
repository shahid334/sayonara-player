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
#include "GUI/ui_GUI_Player.h"
#include "GUI/playlist/GUI_Playlist.h"
#include "GUI/library/GUI_Library_windowed.h"
#include "GUI/library/GUI_LibraryPath.h"
#include "GUI/equalizer/GUI_Equalizer.h"
#include "GUI/playlist_chooser/GUI_PlaylistChooser.h"
#include "GUI/stream/GUI_Stream.h"
#include "GUI/Podcasts/GUI_Podcasts.h"
#include "GUI/alternate_covers/GUI_Alternate_Covers.h"
#include "GUI/Notifications/GUI_Notifications.h"
#include "GUI/startup_dialog/GUI_Startup_Dialog.h"
#include "GUI/LanguageChooser/GUI_LanguageChooser.h"
#include "GUI/player/GUI_TrayIcon.h"

#include "CoverLookup/CoverLookup.h"
#include "Notification/Notification.h"
#include "PlayerPlugin/PlayerPluginHandler.h"
#include "PlayerPlugin/PlayerPlugin.h"

#include "HelperStructs/AsyncWebAccess.h"

#include <QMainWindow>
#include <QCloseEvent>
#include <QTranslator>
#include <QCloseEvent>
#include <QKeySequence>
#include <QTranslator>
#include <QAction>


class GUI_Player : public QMainWindow, private Ui::Sayonara
{
    Q_OBJECT
public:
    explicit GUI_Player(QTranslator* translator, QWidget *parent = 0);
    ~GUI_Player();

public slots:

	void cover_found(const CoverLocation&);
    void update_track (const MetaData & in, int pos=0, bool playing=true);
    void setCurrentPosition (quint32 pos_sec);
    void psl_id3_tags_changed(MetaDataList& v_md);
	void psl_track_time_changed(const MetaData&);
	void psl_bitrate_changed(qint32);
    void psl_reload_library_allowed(bool);
    void psl_set_status_bar_text(QString, bool show);

    void really_close(bool=false);

    /* Last FM */
    void last_fm_logged_in(bool);
    void lfm_info_fetched(const MetaData& md, bool loved, bool corrected);
    void psl_lfm_activated(bool b);

    /**
      * Set current position in filestream
      */
    void psl_strrip_set_active(bool);
    void setVolume(int vol);
    void trayItemActivated (QSystemTrayIcon::ActivationReason reason);
    void stopped();


    /* Plugins */

    void showPlugin(PlayerPlugin* plugin);
    void hideAllPlugins();


    void psl_libpath_changed(QString &);


signals:

    /* Player*/
    void sig_play();
    void sig_pause();
    void sig_stop();
    void sig_backward();
    void sig_forward();
    void sig_mute();
    void sig_rec_button_toggled(bool);
    void sig_volume_changed (int);
	void sig_seek_rel(quint32 pos_percent);
    void sig_seek_rel_ms(qint64 ms);
    void sig_correct_id3(const MetaData&);

    /* File */
    void sig_file_selected (QStringList & filelist);
    void sig_basedir_selected (const QString & baseDir);
    void sig_import_dir(const QString&);
    void sig_import_files(const QStringList&);
    void sig_reload_library(bool clear);
    void sig_clear_library();

    /* Preferences / View */
    void sig_show_playlists();
    void sig_show_small_playlist_items(bool);
    void sig_show_socket();
    void sig_show_stream_rec();
    void sig_libpath_changed(QString);
    void sig_setup_LastFM();
    void sig_skin_changed(bool);
    void sig_show_only_tracks(bool);
    void sig_language_changed();

    /* Covers */
    void sig_want_cover(const MetaData &);
    void sig_fetch_alternate_covers(int);
    void sig_want_more_covers();
    void sig_fetch_all_covers();

    void sig_stream_selected(const QString&, const QString&);



private slots:

    void playClicked(bool b = true);
    void stopClicked(bool b = true);
    void backwardClicked(bool b = true);
    void forwardClicked(bool b = true);
    void sl_rec_button_toggled(bool b);
    void correct_btn_clicked(bool b=false);
    void coverClicked();
    void setProgressJump(int percent);
    void jump_forward();
    void jump_backward();
    void jump_forward_ms();
    void jump_backward_ms();

    void muteButtonPressed();
    void volumeChanged(int volume_percent);
    void volumeChangedByTick(int val);
    void volumeHigher();
    void volumeLower();

    /* File */
    void fileSelectedClicked(bool);
    void folderSelectedClicked(bool);
    void reloadLibraryClicked(bool b = true);
    void importFolderClicked(bool b = true);
    void importFilesClicked(bool b = true);

    /* View */
    void showLibrary(bool, bool resize=true);
    void changeSkin(bool);
    void small_playlist_items_toggled(bool);
    void show_notification_toggled(bool);
    void show_fullscreen_toggled(bool);


    /* Preferences */
    void sl_action_language_toggled(bool b=true);
    void lastFMClicked(bool b = true);
    void setLibraryPathClicked(bool b = true);

    void load_pl_on_startup_toggled(bool);
    void min2tray_toggled(bool);
    void only_one_instance_toggled(bool);
    void sl_action_streamripper_toggled(bool);
    void sl_action_socket_connection_triggered(bool);
    void sl_show_only_tracks(bool);
    void sl_live_search(bool);
    void sl_notify_new_version(bool);

    void about(bool b=false);
    void help(bool b=false);


	void sl_alternate_cover_available(const CoverLocation&);
    void sl_no_cover_available();

    void awa_version_finished();
    void awa_translators_finished();


    void notification_changed(bool active, int ms);
    void language_changed(QString);


public:
    void setPlaylist(GUI_Playlist* playlist);
    void setLibrary(GUI_Library_windowed* library);
    void setInfoDialog(GUI_InfoDialog* info_dialog);
    void setPlayerPluginHandler(PlayerPluginHandler* pph);

    QWidget* getParentOfPlaylist();
    QWidget* getParentOfLibrary();
    QWidget* getParentOfPlugin();

    void ui_loaded();
    void setStyle(int);


protected:

    void closeEvent(QCloseEvent* e);

    void keyPressEvent(QKeyEvent* e);
    void resizeEvent(QResizeEvent* e);
    void moveEvent(QMoveEvent* e);

private:

    GUI_Playlist*           ui_playlist;
    GUI_Library_windowed*   ui_library;
    GUI_LibraryPath*        ui_libpath;
    GUI_InfoDialog*         ui_info_dialog;
    GUI_Notifications*      ui_notifications;
    GUI_Startup_Dialog*     ui_startup_dialog;
    GUI_LanguageChooser*    ui_language_chooser;
    CoverLookup*            m_cov_lookup;
    PlayerPluginHandler*    _pph;

    GUI_Alternate_Covers*   m_alternate_covers;
    AsyncWebAccess*         m_awa_version;
    AsyncWebAccess*         m_awa_translators;

    QString                 m_class_name;
	qint64                 m_completeLength_ms;
    bool                    m_playing;
    bool                    m_mute;
    GUI_TrayIcon *          m_trayIcon;

    QString                 m_skinSuffix;

    MetaData			m_metadata;
    MetaData			m_metadata_corrected;
    bool                m_metadata_available;
    bool                m_min2tray;

    int                 m_library_width;
    int                 m_library_stretch_factor;
    CSettingsStorage*   m_settings;
    QTranslator*        m_translator;
    QStringList         m_translators;

	bool				m_converter_active;




    void setupTrayActions ();

    void setupVolButton(int percent);
    void initGUI();
    void setupConnections();
    void setRadioMode(int);
    void total_time_changed(qint64);
	void set_std_cover(bool radio);

    void fetch_cover();
    QAction* createAction(QKeySequence key_sequence);
    QAction* createAction(QList<QKeySequence>& key_sequences);

};


#endif // GUI_SIMPLEPLAYER_H
