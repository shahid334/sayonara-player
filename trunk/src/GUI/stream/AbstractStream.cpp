#include "GUI/stream/AbstractStream.h"

#include "HelperStructs/Helper.h"
#include "HelperStructs/MetaData/MetaData.h"
#include "HelperStructs/Parser/PlaylistParser.h"
#include "HelperStructs/Parser/PodcastParser.h"
#include <QMessageBox>


AbstractStream::AbstractStream(QString name, QWidget* parent) :
	PlayerPlugin(name, parent)
{
	_cur_station_idx = -1;
	_playlist = PlaylistHandler::getInstance();
	_db = CDatabaseConnector::getInstance();


	hide();
}

AbstractStream::~AbstractStream(){

}

void AbstractStream::init_connections(){
	connect(_btn_play,	SIGNAL(clicked()),						this, SLOT(listen_clicked()));
	connect(_btn_tool,		SIGNAL(sig_save()),						this, SLOT(save_clicked()));
	connect(_btn_tool,		SIGNAL(sig_delete()),					this, SLOT(delete_clicked()));
	connect(_combo_stream,	SIGNAL(currentIndexChanged(int)),		this, SLOT(combo_idx_changed(int)));
	connect(_combo_stream,	SIGNAL(editTextChanged(const QString&)),this, SLOT(combo_text_changed(const QString&)));
	connect(_le_url, SIGNAL(textEdited(const QString&)),			this, SLOT(url_text_changed(const QString&)));

}


void AbstractStream::language_changed(){

}

void AbstractStream::play(QString url, QString station_name){

	qDebug() << "Play stream: " << station_name;
	QString content;

	MetaDataList v_md;

	bool post_proc = false;

	if(Helper::is_podcastfile(url, &content)){
		PodcastParser::parse_podcast_xml_file(url, v_md);
		post_proc = true;
	}

	else if(Helper::is_playlistfile(url)) {
		PlaylistParser::parse_playlist(url, v_md);
		post_proc = true;
	}

	if(post_proc){
		for(MetaData& md : v_md) {

			if(station_name.isEmpty()){
				md.album = url;
				if(md.title.isEmpty()){
					md.title = _title_fallback_name;
				}
			}

			else{
				md.album = station_name;
				if(md.title.isEmpty()){
					md.title = station_name;
				}
			}

			if(md.artist.isEmpty()){
				md.artist = url;
			}
		}
	}

	// real stream
	else{

		MetaData md;

		if(station_name.isEmpty()){
			md.title = _title_fallback_name;
			md.album = url;
		}

		else{
			md.title = station_name;
			md.album = station_name;
		}

		md.artist = url;
		md.set_filepath(url);

		v_md.push_back(md);
	}

	_playlist->create_playlist(v_md, station_name, false, PlaylistTypeStream);
}


void AbstractStream::listen_clicked(){
	QString url;
	QString name;

	if(_cur_station_idx == -1) {
		url = _le_url->text();
		name = _title_fallback_name;
	}

	else{
		url = _cur_station_adress;
		name = _cur_station_name;
	}

	url = url.trimmed();
	if(url.size() > 5) {

		play(url, name);
	}
}

void AbstractStream::combo_idx_changed(int idx){
	_cur_station_idx = idx;
	_cur_station_name = _combo_stream->itemText(_cur_station_idx);

	QString adress = _stations[_cur_station_name];
	if(adress.size() > 0) {
		_cur_station_adress = adress;
		_le_url->setText(_cur_station_adress);
	}

	if(idx == 0) {
		_le_url->setText("");
	}

	_btn_tool->show_delete(idx > 0);
	_btn_tool->show_save(false);

	bool listen_enabled = (_le_url->text().size() > 5);
	_btn_play->setEnabled(listen_enabled);
	_lab_listen->setEnabled(listen_enabled);
	_combo_stream->setToolTip(_cur_station_adress);
}

void AbstractStream::combo_text_changed(const QString& text){
	_cur_station_idx = -1;

	bool name_there = false;

	for( int i=0; i<_combo_stream->count(); i++ ) {

		QString str = _combo_stream->itemText(i);

		if( str.compare(text) == 0) {
			name_there = true;
			break;
		}
	}

	_btn_tool->show_delete(name_there);
	_btn_tool->show_save(text.size() > 0);

	bool listen_enabled = (_le_url->text().size() > 5);
	_btn_play->setEnabled(listen_enabled);
	_lab_listen->setEnabled(listen_enabled);
	_combo_stream->setToolTip("");
}

void AbstractStream::url_text_changed(const QString& url){

	QString key = _stations.key(url.trimmed());

	if(! key.isEmpty() ) {

		int idx = _combo_stream->findText(key, Qt::MatchCaseSensitive);

		if(idx != -1) {

			_combo_stream->setCurrentIndex(idx);
			_btn_tool->show_save(false);
			_btn_tool->show_delete(true);

			_cur_station_idx = idx;
		}
	}

	// new adress
	else{

		_btn_tool->show_delete(false);

		bool save_enabled =
				_combo_stream->currentText().size() > 0 &&
				_le_url->text().size() > 5 &&
				_cur_station_idx == -1;

		_btn_tool->show_save(save_enabled);
		bool listen_enabled = (_le_url->text().size() > 5);
		_btn_play->setEnabled(listen_enabled);
		_lab_listen->setEnabled(listen_enabled);

		if(_cur_station_idx != -1) {
			_cur_station_idx = -1;
			_combo_stream->setEditText(tr("new"));
			_cur_station_idx = -1;
		}
	}
}

int	AbstractStream::show_delete_confirm_dialog(){

	QMessageBox msgBox(this);
	QString ask = tr("Really wanna delete %1?").arg(_cur_station_name);
	msgBox.setText(ask );
	msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
	msgBox.setModal(true);
	msgBox.setIcon(QMessageBox::Information);

	return msgBox.exec();
}

void AbstractStream::delete_clicked(){

	if(_cur_station_idx == -1) return;

	int ret = show_delete_confirm_dialog();

	if(ret == QMessageBox::Yes) {
		if( delete_stream(_cur_station_name) ) {
			qDebug() << _cur_station_name << "successfully deleted";
			QMap<QString, QString> map;
			if( get_all_streams(map) ) {
				setup_stations(map);
			}
		}
	}

	_cur_station_idx = -1;
}

void AbstractStream::save_clicked(){

	QString name = _combo_stream->currentText();
	QString url = _le_url->text();

	bool success = false;
	if(name.size() > 0 && url.size() > 0) {
		success = add_stream(name, url);
	}

	if(success) {
		QMap<QString, QString> map;
		if( get_all_streams(map)) {
			setup_stations(map);
			for(int i=0; i<_combo_stream->count(); i++){
				QString s = _combo_stream->itemText(i);
				if(s.compare(name) == 0){
					_combo_stream->setCurrentIndex(i);
					break;
				}
			}
		}
	}

	_cur_station_idx = -1;
	_le_url->setText(url);

	url_text_changed(url);
}



void AbstractStream::setup_stations(const QMap<QString, QString>& stations){

	_combo_stream->clear();

	_stations = stations;
	if(stations.size() > 0) {
		_cur_station_idx = -1;
	}

	_cur_station_adress = "";
	_cur_station_name = "";
	_cur_station_idx = 0;

	_stations[""] = "";

	QMap<QString, QString>::iterator it;
	for(it = _stations.begin(); it != _stations.end(); it++) {
		_combo_stream->addItem(it.key(), it.value());
	}

	_btn_play->setEnabled(false);
	_lab_listen->setEnabled(false);
	_btn_tool->show_save(false);
	_btn_tool->show_delete(false);
}

