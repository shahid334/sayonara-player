/*
 * GUI_Library_windowed.h
 *
 *  Created on: Apr 24, 2011
 *      Author: luke
 */

#ifndef GUI_LIBRARY_WINDOWED_H_
#define GUI_LIBRARY_WINDOWED_H_

#include <QObject>
#include <QWidget>
#include <QAbstractTableModel>
#include <QPoint>

#include <ui_GUI_Library_windowed.h>
#include <GUI/library/LibraryItemModelTracks.h>
#include <GUI/library/LibraryItemModelAlbums.h>
#include <GUI/library/LibraryItemDelegateAlbums.h>
#include <GUI/library/LibraryItemDelegateArtists.h>
#include <GUI/library/LibraryItemModelArtists.h>
#include <GUI/MyTableView.h>
#include <HelperStructs/MetaData.h>
#include <DatabaseAccess/CDatabaseConnector.h>

#include <QMenu>
#include <QMessageBox>

#include <vector>

using namespace std;



class GUI_Library_windowed : public QWidget, private Ui::Library_windowed{

Q_OBJECT

public:
	GUI_Library_windowed(QWidget* parent = 0);
	virtual ~GUI_Library_windowed();

private:
	Ui::Library_windowed* ui;
	LibraryItemModelTracks* _track_model;
	LibraryItemModelAlbums* _album_model;
	LibraryItemDelegateAlbums* _album_delegate;
	LibraryItemModelArtists* _artist_model;
	LibraryItemDelegateArtists* _artist_delegate;
	vector<MetaData>	_v_metadata;
	vector<Album>		_v_albums;
	vector<Artist>		_v_artists;

	QMenu*				_right_click_menu;
	QAction*			_info_action;
	QAction*			_edit_action;
	QAction*			_delete_action;

	QMessageBox*		_album_msg_box;



	signals:
		void artist_changed_signal(int);
		void album_changed_signal(int);
		void clear_signal();
		void album_chosen_signal(vector<MetaData>&);
		void artist_chosen_signal(vector<MetaData>&);
		void track_chosen_signal(vector<MetaData>& );
		void reload_library();
		void data_for_id3_change(const vector<MetaData>&);
		void search_cover(const MetaData&);

	public slots:
		void fill_library_tracks(vector<MetaData>&);
		void fill_library_albums(vector<Album>&);
		void fill_library_artists(vector<Artist>&);
		void id3_tags_changed();
		void reloading_library();
		void reloading_library_finished();
		void library_should_be_reloaded();
		void cover_changed(bool);


	private slots:
		void track_pressed(const QModelIndex&);
		void album_pressed(const QModelIndex &);
		void artist_pressed(const QModelIndex &);

		void clear_button_pressed();
		void album_chosen(const QModelIndex & );
		void artist_chosen(const QModelIndex & );
		void track_chosen(const QModelIndex & );

		void text_line_edited(const QString&);
		void sort_by_column(int col);

		void reload_library_slot();

		void show_artist_context_menu(const QPoint& p);
		void show_album_context_menu(const QPoint& p);
		void show_track_context_menu(const QPoint& p);

		void edit_album();
		void info_album();
		void delete_album();
		void edit_artist();
		void info_artist();
		void delete_artist();
		void edit_tracks();
		void info_tracks();
		void delete_tracks();

		void apply_cover_to_entire_album();


	protected:
			void resizeEvent(QResizeEvent* e);


	public:
			void change_skin(bool dark);

	private:
			QString getTotalTimeString(Album& album);
			void init_menues();
			QString _sort_albums;

			int _selected_artist;
			int _selected_album;

			Album _album_of_interest;

			bool _everything_loaded;

};

#endif /* GUI_LIBRARY_WINDOWED_H_ */
