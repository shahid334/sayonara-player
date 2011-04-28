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

#include <ui_GUI_Library_windowed.h>
#include <GUI/Library/LibraryItemModelTracks.h>
#include <GUI/Library/LibraryItemModelAlbums.h>
#include <GUI/Library/LibraryItemModelArtists.h>
#include <HelperStructs/MetaData.h>
#include <library/CDatabaseConnector.h>

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
	LibraryItemModelArtists* _artist_model;
	vector<MetaData>	_v_metadata;
	vector<Album>		_v_albums;
	vector<Artist>		_v_artists;


	signals:
		void artist_changed_signal(int);
		void album_changed_signal(int);
		void clear_signal();
		void album_chosen_signal(vector<MetaData>&);
		void artist_chosen_signal(vector<MetaData>&);

	public slots:
		void fill_library_tracks(vector<MetaData>&);
		void fill_library_albums(vector<Album>&);
		void fill_library_artists(vector<Artist>&);


	private slots:
		void track_pressed(const QModelIndex&);
		void album_pressed(const QModelIndex &);
		void artist_pressed(const QModelIndex &);

		void clear_button_pressed();
		void album_chosen(const QModelIndex & );
		void artist_chosen(const QModelIndex & );

	protected:
			void resizeEvent(QResizeEvent* e);





};

#endif /* GUI_LIBRARY_WINDOWED_H_ */
