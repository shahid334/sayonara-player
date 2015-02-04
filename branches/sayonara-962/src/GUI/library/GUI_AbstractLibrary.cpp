#include "GUI/library/GUI_AbstractLibrary.h"
#include <QMessageBox>

GUI_AbstractLibrary::GUI_AbstractLibrary(AbstractLibrary* library, GUI_InfoDialog* info_dialog, QWidget *parent) :
	SayonaraWidget(parent)
{
	_library = library;
	_info_dialog = info_dialog;

	_shown_cols_albums = _settings->get(Set::Lib_ColsAlbum);
	_shown_cols_artist = _settings->get(Set::Lib_ColsArtist);
	_shown_cols_tracks = _settings->get(Set::Lib_ColsTitle);
}

GUI_AbstractLibrary::~GUI_AbstractLibrary(){
	delete _album_model;
	delete _album_delegate;
	delete _artist_model;
	delete _artist_delegate;
	delete _track_model;
	delete _track_delegate;
}

void GUI_AbstractLibrary::init_finished(){

	connect(_library, SIGNAL(sig_all_artists_loaded(const ArtistList&)), this, SLOT(lib_fill_artists(const ArtistList&)));
	connect(_library, SIGNAL(sig_all_albums_loaded(const AlbumList&)), this, SLOT(lib_fill_albums(const AlbumList&)));
	connect(_library, SIGNAL(sig_all_tracks_loaded(const MetaDataList&)), this,	SLOT(lib_fill_tracks(const MetaDataList&)));
	connect(_library, SIGNAL(sig_track_mime_data_available(const MetaDataList&)), this, SLOT(track_info_available(const MetaDataList&)));
	connect(_library, SIGNAL(sig_delete_answer(QString)), this, SLOT(show_delete_answer(QString)));
	connect(_library, SIGNAL(sig_reloading_library(const QString&)), this, SLOT(lib_reload(const QString&)));
	connect(_library, SIGNAL(sig_reload_library_finished()), this, SLOT(reload_finished()));


	connect(_lv_album, SIGNAL(doubleClicked(const QModelIndex & )), this, SLOT(album_dbl_clicked(const QModelIndex & )));
	connect(_lv_album, SIGNAL(sig_sel_changed(const QList<int> & )), this, SLOT(album_sel_changed(const QList<int>&)));
	connect(_lv_album, SIGNAL(sig_middle_button_clicked(const QPoint&)), this, SLOT(album_middle_clicked(const QPoint&)));
	connect(_lv_album, SIGNAL(sig_sortorder_changed(Sort::SortOrder)), this, SLOT(sortorder_album_changed(Sort::SortOrder)));
	connect(_lv_album, SIGNAL(sig_columns_changed(QList<int>&)), this, SLOT(columns_album_changed(QList<int>&)));
	connect(_lv_album, SIGNAL(sig_edit_clicked()), this, SLOT(edit_album()));
	connect(_lv_album, SIGNAL(sig_info_clicked()), this, SLOT(info_album()));
	connect(_lv_album, SIGNAL(sig_delete_clicked()), this, SLOT(delete_album()));
	connect(_lv_album, SIGNAL(sig_play_next_clicked()), this, SLOT(play_next()));
	connect(_lv_album, SIGNAL(sig_append_clicked()), this, SLOT(append()));

	connect(_lv_artist, SIGNAL(doubleClicked(const QModelIndex & )), this, SLOT(artist_dbl_clicked(const QModelIndex & )));
	connect(_lv_artist, SIGNAL(sig_sel_changed(const QList<int> & )), this, SLOT(artist_sel_changed(const QList<int>&)));
	connect(_lv_artist, SIGNAL(sig_middle_button_clicked(const QPoint&)), this, SLOT(artist_middle_clicked(const QPoint&)));
	connect(_lv_artist, SIGNAL(sig_sortorder_changed(Sort::SortOrder)), this, SLOT(sortorder_artist_changed(Sort::SortOrder)));
	connect(_lv_artist, SIGNAL(sig_columns_changed(QList<int>&)), this, SLOT(columns_artist_changed(QList<int>&)));
	connect(_lv_artist, SIGNAL(sig_edit_clicked()), this, SLOT(edit_artist()));
	connect(_lv_artist, SIGNAL(sig_info_clicked()), this, SLOT(info_artist()));
	connect(_lv_artist, SIGNAL(sig_delete_clicked()), this, SLOT(delete_artist()));
	connect(_lv_artist, SIGNAL(sig_play_next_clicked()), this, SLOT(play_next()));
	connect(_lv_artist, SIGNAL(sig_append_clicked()), this, SLOT(append()));

	connect(_lv_tracks, SIGNAL(doubleClicked(const QModelIndex & )), this, SLOT(track_dbl_clicked(const QModelIndex & )));
	connect(_lv_tracks, SIGNAL(sig_sel_changed(const QList<int> & )), this, SLOT(track_sel_changed(const QList<int>&)));
	connect(_lv_tracks, SIGNAL(sig_middle_button_clicked(const QPoint&)), this, SLOT(tracks_middle_clicked(const QPoint&)));
	connect(_lv_tracks, SIGNAL(sig_sortorder_changed(Sort::SortOrder)), this, SLOT(sortorder_title_changed(Sort::SortOrder)));
	connect(_lv_tracks, SIGNAL(sig_columns_changed(QList<int>&)), this, SLOT(columns_title_changed(QList<int>&)));
	connect(_lv_tracks, SIGNAL(sig_edit_clicked()), this, SLOT(edit_tracks()));
	connect(_lv_tracks, SIGNAL(sig_info_clicked()), this, SLOT(info_tracks()));
	connect(_lv_tracks, SIGNAL(sig_delete_clicked()), this, SLOT(delete_tracks()));
	connect(_lv_tracks, SIGNAL(sig_play_next_clicked()), this, SLOT(play_next_tracks()));
	connect(_lv_tracks, SIGNAL(sig_append_clicked()), this, SLOT(append_tracks()));

	init_headers();

	connect(_btn_refresh, SIGNAL(clicked()), this, SLOT(refresh()));

	connect(_btn_clear, SIGNAL( clicked()), this, SLOT(clear_button_pressed()));

	connect(_le_search, SIGNAL( textEdited(const QString&)), this, SLOT(text_line_edited(const QString&)));
	connect(_le_search, SIGNAL(returnPressed()), this, SLOT(return_pressed()));
	connect(_combo_search, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_search_changed(int)));

	REGISTER_LISTENER(Set::Lib_OnlyTracks, _sl_show_only_tracks_changed);
}

void GUI_AbstractLibrary::language_changed(){

	_header_names_tracks.clear();
	_header_names_albums.clear();
	_header_names_artists.clear();

	_header_names_tracks << "#" << tr("Title") << tr("Artist") << tr("Album") << tr("Year") << tr("Dur.") << tr("Bitrate") << tr("Filesize");
	_header_names_albums << "#" << tr("Album") << tr("Duration") << tr("#Tracks") << tr("Year");
	_header_names_artists << "#" << tr("Artist") << tr("#Tracks");

	_album_model->set_new_header_names(_header_names_albums);
	_artist_model->set_new_header_names(_header_names_artists);
	_track_model->set_new_header_names(_header_names_tracks);
}

void GUI_AbstractLibrary::init_headers(){

	LibSortOrder so = _settings->get(Set::Lib_Sorting);

	_header_names_tracks << "#" << tr("Title") << tr("Artist") << tr("Album") << tr("Year") << tr("Dur.") << tr("Bitrate") << tr("Filesize") << tr("Rating");
	_header_names_albums << "#" << tr("Album") << tr("Duration") << tr("#Tracks") << tr("Year") << tr("Rating");
	_header_names_artists << "#" << tr("Artist") << tr("#Tracks");

	ColumnHeader t_h0(_header_names_tracks[0], true, Sort::TrackNumAsc, Sort::TrackNumDesc, 25);
	ColumnHeader t_h1(_header_names_tracks[1], false, Sort::TrackTitleAsc, Sort::TrackTitleDesc, 0.4, 200);
	ColumnHeader t_h2(_header_names_tracks[2], true, Sort::TrackArtistAsc, Sort::TrackArtistDesc, 0.3, 160);
	ColumnHeader t_h3(_header_names_tracks[3], true, Sort::TrackAlbumAsc, Sort::TrackAlbumDesc, 0.3, 160);
	ColumnHeader t_h4(_header_names_tracks[4], true, Sort::TrackYearAsc, Sort::TrackYearDesc, 50);
	ColumnHeader t_h5(_header_names_tracks[5], true, Sort::TrackLenghtAsc, Sort::TrackLengthDesc, 50);
	ColumnHeader t_h6(_header_names_tracks[6], true, Sort::TrackBitrateAsc, Sort::TrackBitrateDesc, 75);
	ColumnHeader t_h7(_header_names_tracks[7], true, Sort::TrackSizeAsc, Sort::TrackSizeDesc, 75);
	ColumnHeader t_h8(_header_names_tracks[8], true, Sort::TrackRatingAsc, Sort::TrackRatingDesc, 50);

	ColumnHeader al_h0(_header_names_albums[0], true, Sort::NoSorting, Sort::NoSorting, 20);
	ColumnHeader al_h1(_header_names_albums[1], false, Sort::AlbumNameAsc, Sort::AlbumNameDesc, 1.0, 160);
	ColumnHeader al_h2(_header_names_albums[2], true, Sort::AlbumDurationAsc, Sort::AlbumDurationDesc, 90);
	ColumnHeader al_h3(_header_names_albums[3], true, Sort::AlbumTracksAsc, Sort::AlbumTracksDesc, 80);
	ColumnHeader al_h4(_header_names_albums[4], true, Sort::AlbumYearAsc, Sort::AlbumYearDesc, 50);
	ColumnHeader al_h5(_header_names_albums[5], true, Sort::AlbumRatingAsc, Sort::AlbumRatingDesc, 50);

	ColumnHeader ar_h0(_header_names_artists[0], true, Sort::NoSorting, Sort::NoSorting, 20);
	ColumnHeader ar_h1(_header_names_artists[1], false, Sort::ArtistNameAsc, Sort::ArtistNameDesc, 1.0, 160 );
	ColumnHeader ar_h2(_header_names_artists[2], true, Sort::ArtistTrackcountAsc, Sort::ArtistTrackcountDesc, 80);

	_track_columns  << t_h0  << t_h1  << t_h2  << t_h3  <<t_h4  << t_h5  << t_h6 << t_h7 << t_h8;
	_album_columns  << al_h0 << al_h1 << al_h2 << al_h3 << al_h4 << al_h5;
	_artist_columns << ar_h0 << ar_h1 << ar_h2;

	_album_model = new LibraryItemModelAlbums(_album_columns);
	_artist_model = new LibraryItemModelArtists(_artist_columns);
	_track_model = new LibraryItemModelTracks(_track_columns);


	_album_delegate = new LibraryItemDelegateAlbums(_lv_album, true);
	_artist_delegate = new LibraryItemDelegateArtists(_lv_artist);
	_track_delegate = new LibraryItemDelegateTracks(_lv_tracks, true);

	connect(_album_delegate, SIGNAL(sig_rating_changed(int)), this, SLOT(album_rating_changed(int)));
	connect(_track_delegate, SIGNAL(sig_rating_changed(int)), this, SLOT(title_rating_changed(int)));

	_lv_tracks->setModel(_track_model);
	_lv_tracks->setAbstractModel((AbstractSearchTableModel*) _track_model);
	_lv_tracks->setItemDelegate(_track_delegate);
	_lv_tracks->setAlternatingRowColors(true);
	_lv_tracks->setDragEnabled(true);
	_lv_tracks->set_table_headers(_track_columns, so.so_tracks);
	_lv_tracks->rc_header_menu_init(_shown_cols_tracks);


	_lv_artist->setModel(_artist_model);
	_lv_artist->setAbstractModel((AbstractSearchTableModel*) _artist_model);
	_lv_artist->setItemDelegate(_artist_delegate);
	_lv_artist->setAlternatingRowColors(true);
	_lv_artist->setDragEnabled(true);
	_lv_artist->set_table_headers(_artist_columns, so.so_artists);
	_lv_artist->rc_header_menu_init(_shown_cols_artist);


	_lv_album->setModel(_album_model);
	_lv_album->setAbstractModel((AbstractSearchTableModel*) _album_model);
	_lv_album->setItemDelegate(_album_delegate);
	_lv_album->setAlternatingRowColors(true);
	_lv_album->setDragEnabled(true);
	_lv_album->set_table_headers(_album_columns, so.so_albums);
	_lv_album->rc_header_menu_init(_shown_cols_albums);


}


void GUI_AbstractLibrary::text_line_edited(const QString &search, bool force_emit){

	bool live_search = _settings->get(Set::Lib_LiveSearch);

	if(!force_emit && !live_search ) return;

	if(search.startsWith("f:", Qt::CaseInsensitive)) {
		_combo_search->setCurrentIndex(0);
		_le_search->setText("");
	}

	else if(search.startsWith("g:", Qt::CaseInsensitive)) {
		_combo_search->setCurrentIndex(1);
		_le_search->setText("");
	}

	else if(search.startsWith("p:", Qt::CaseInsensitive)) {
		_combo_search->setCurrentIndex(2);
		_le_search->setText("");
	}

	Filter filter;
	int idx = _combo_search->currentIndex();
	switch(idx) {
		case 0:	filter.by_searchstring = BY_FULLTEXT; break;
		case 1: filter.by_searchstring = BY_GENRE; break;
		case 2: filter.by_searchstring = BY_FILENAME; break;

		default: filter.by_searchstring = BY_FULLTEXT; break;
	}

	if(search.size() < 3)
		filter.cleared = true;

	else{
		filter.filtertext = QString("%") + search + QString("%");
		filter.cleared = false;
	}

	_cur_searchfilter = filter;

	_library->psl_filter_changed(filter);
}


void GUI_AbstractLibrary::clear_button_pressed() {

	disconnect(_le_search, SIGNAL( textEdited(const QString&)), this, SLOT(text_line_edited(const QString&)));

	_le_search->setText("");
	_library->refetch();

	connect(_le_search, SIGNAL( textEdited(const QString&)), this, SLOT(text_line_edited(const QString&)));
}


void GUI_AbstractLibrary::combo_search_changed(int idx) {
	Q_UNUSED(idx);
	text_line_edited(_cur_searchfilter.filtertext, true);
}

void GUI_AbstractLibrary::return_pressed() {

	text_line_edited(_le_search->text(), true);
}


void GUI_AbstractLibrary::refresh(){
	_library->refresh();
}


void GUI_AbstractLibrary::lib_fill_tracks(const MetaDataList& v_metadata) {

	_lv_tracks->fill<MetaDataList, MetaData>(v_metadata);

	_lv_artist->set_mimedata(v_metadata, true, true);
	_lv_album->set_mimedata(v_metadata, true);
}


void GUI_AbstractLibrary::lib_fill_albums(const AlbumList& albums) {

   _lv_album->fill<AlbumList, Album>(albums);

}


void GUI_AbstractLibrary::lib_fill_artists(const ArtistList& artists) {

	_lv_artist->fill<ArtistList, Artist>(artists);
}


void GUI_AbstractLibrary::track_info_available(const MetaDataList& v_md) {

	_lv_tracks->set_mimedata(v_md, false);

}




void GUI_AbstractLibrary::artist_sel_changed(const QList<int>& lst) {

	_info_dialog->setInfoMode(InfoDialogMode_Artists);

	_library->psl_selected_artists_changed(lst);
}


void GUI_AbstractLibrary::album_sel_changed(const QList<int>& lst) {

	_info_dialog->setInfoMode(InfoDialogMode_Albums);

	_library->psl_selected_albums_changed(lst);
}

void GUI_AbstractLibrary::track_sel_changed(const QList<int>& lst) {
	_info_dialog->setInfoMode(InfoDialogMode_Tracks);
	_library->psl_selected_tracks_changed(lst);
}


void GUI_AbstractLibrary::artist_middle_clicked(const QPoint& pt) {
	append();
}


void GUI_AbstractLibrary::album_middle_clicked(const QPoint& pt) {
	append();
}

void GUI_AbstractLibrary::tracks_middle_clicked(const QPoint& pt) {
	append_tracks();
}




void GUI_AbstractLibrary::album_dbl_clicked(const QModelIndex & idx) {
	_library->psl_prepare_album_for_playlist(idx.row());
}

void GUI_AbstractLibrary::artist_dbl_clicked(const QModelIndex & idx) {
	_library->psl_prepare_artist_for_playlist(idx.row());
}

void GUI_AbstractLibrary::track_dbl_clicked(const QModelIndex& idx) {

	QList<int> lst = _track_model->get_selected();

	if(lst.size() ==0 ) {
		lst << idx.row();
	}

	_library->psl_prepare_tracks_for_playlist(lst);
}







void  GUI_AbstractLibrary::columns_album_changed(QList<int>& list) {
	_shown_cols_albums = list;
	_settings->set(Set::Lib_ColsAlbum, list);
}


void  GUI_AbstractLibrary::columns_artist_changed(QList<int>& list) {
	_shown_cols_artist = list;
	_settings->set(Set::Lib_ColsArtist, list);
}


void  GUI_AbstractLibrary::columns_title_changed(QList<int> & list) {
	_shown_cols_tracks = list;
	_settings->set(Set::Lib_ColsTitle, list);
}



void GUI_AbstractLibrary::sortorder_artist_changed(Sort::SortOrder s) {

	LibSortOrder so = _settings->get(Set::Lib_Sorting);
	so.so_artists = s;

	 _settings->set(Set::Lib_Sorting, so);
}


void GUI_AbstractLibrary::sortorder_album_changed(Sort::SortOrder s) {

   LibSortOrder so = _settings->get(Set::Lib_Sorting);
	so.so_albums = s;

	_settings->set(Set::Lib_Sorting, so);
}


void GUI_AbstractLibrary::sortorder_title_changed(Sort::SortOrder s) {

	LibSortOrder so = _settings->get(Set::Lib_Sorting);
	so.so_tracks = s;

	 _settings->set(Set::Lib_Sorting, so);
}


void GUI_AbstractLibrary::edit_album() {
	if(!_info_dialog) return;

	_info_dialog->show(TAB_EDIT);
}

void GUI_AbstractLibrary::edit_artist() {
	if(!_info_dialog) return;

	_info_dialog->show(TAB_EDIT);
}

void GUI_AbstractLibrary::edit_tracks() {
	if(!_info_dialog) return;

	_info_dialog->show(TAB_EDIT);
}


void GUI_AbstractLibrary::info_album() {
	if(!_info_dialog) return;

	_info_dialog->show(TAB_INFO);
}

void GUI_AbstractLibrary::info_artist() {
	if(!_info_dialog) return;

	_info_dialog->show(TAB_INFO);
}

void GUI_AbstractLibrary::info_tracks() {
	if(!_info_dialog) return;

	_info_dialog->show(TAB_INFO);
}



void GUI_AbstractLibrary::delete_album() {

	int n_tracks = _track_model->rowCount();
	AbstractLibrary::TrackDeletionMode answer = show_delete_dialog(n_tracks);

	_library->delete_current_tracks(answer);

}

void GUI_AbstractLibrary::delete_artist() {

	int n_tracks = _track_model->rowCount();
	AbstractLibrary::TrackDeletionMode answer = show_delete_dialog(n_tracks);

	_library->delete_current_tracks(answer);
}

void GUI_AbstractLibrary::delete_tracks(){

	QModelIndexList idx_list = _lv_tracks->selectionModel()->selectedRows(0);
	QList<int> lst;
	for(const QModelIndex& idx : idx_list) {
		lst.push_back(idx.row());
	}

	AbstractLibrary::TrackDeletionMode answer = show_delete_dialog(lst.size());

	if(answer){
		_library->delete_tracks_by_idx(lst, answer);
	}
}


void GUI_AbstractLibrary::album_rating_changed(int rating) {
	QList<int> idxs = _album_model->get_selected();
	if(idxs.size() == 0) return;

	_library->psl_album_rating_changed(idxs[0], rating);
}


void GUI_AbstractLibrary::title_rating_changed(int rating) {

	QList<int> idxs = _track_model->get_selected();
	if(idxs.size() == 0) return;

	_library->psl_track_rating_changed(idxs[0], rating);
}





void GUI_AbstractLibrary::append() {

	_library->psl_append_all_tracks();
}


void GUI_AbstractLibrary::append_tracks() {
	QModelIndexList idx_list = _lv_tracks->selectionModel()->selectedRows(0);
	QList<int> lst;
	for(const QModelIndex&  idx : idx_list) {
		lst.push_back(idx.row());
	}

	_library->psl_append_tracks(lst);
}


void GUI_AbstractLibrary::play_next() {

	_library->psl_play_next_all_tracks();
}


void GUI_AbstractLibrary::play_next_tracks() {
	QModelIndexList idx_list = _lv_tracks->selectionModel()->selectedRows(0);
	QList<int> lst;
	for(const QModelIndex& idx : idx_list) {
		lst.push_back(idx.row());
	}

	_library->psl_play_next_tracks(lst);
}


void GUI_AbstractLibrary::lib_reload(const QString& str) {

	QString final_str = QString("<b>") + str + "</b>";
	_lab_status->setText(final_str);
}


void GUI_AbstractLibrary::reload_finished() {
	_lab_status->setText("");
	refresh();
}

void GUI_AbstractLibrary::id3_tags_changed() {
	refresh();
}


void GUI_AbstractLibrary::show_delete_answer(QString answer) {
	QMessageBox answerbox(this);

	answerbox.setText(tr("Info"));
	answerbox.setIcon(QMessageBox::Information);
	answerbox.setInformativeText(answer);

	answerbox.exec();
	answerbox.close();
}


void GUI_AbstractLibrary::_sl_show_only_tracks_changed() {

	bool b = _settings->get(Set::Lib_OnlyTracks);

	_lv_artist->setVisible(!b);
	_lv_album->setVisible(!b);
}



void GUI_AbstractLibrary::resizeEvent(QResizeEvent* e) {

	_lv_album->set_col_sizes();
	_lv_artist->set_col_sizes();
	_lv_tracks->set_col_sizes();
}

