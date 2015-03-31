#include "GUI/stream/AbstractStream.h"

#include "HelperStructs/Helper.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/PlaylistParser.h"

#include <QDebug>
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
	connect(_btn_listen,	SIGNAL(clicked()),						this, SLOT(listen_clicked()));
	connect(_btn_save,		SIGNAL(clicked()),						this, SLOT(save_clicked()));
	connect(_btn_delete,	SIGNAL(clicked()),						this, SLOT(delete_clicked()));
	connect(_combo_stream,	SIGNAL(currentIndexChanged(int)),		this, SLOT(combo_idx_changed(int)));
	connect(_combo_stream,	SIGNAL(editTextChanged(const QString&)),this, SLOT(combo_text_changed(const QString&)));
	connect(_le_url, SIGNAL(textEdited(const QString&)),			this, SLOT(url_text_changed(const QString&)));

}


void AbstractStream::language_changed(){

}

void AbstractStream::play(QString url, QString station_name){

	qDebug() << "Play stream: " << station_name;

	MetaDataList v_md;

	if(Helper::is_playlistfile(url)) {
		if(PlaylistParser::parse_playlist(url, v_md) > 0) {

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

	_playlist->create_playlist(v_md, station_name);
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

	_btn_delete->setEnabled(idx > 0);
	_btn_save->setEnabled(false);
	_btn_listen->setEnabled(_le_url->text().size() > 5);
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

	_btn_delete->setEnabled(name_there);
	_btn_save->setEnabled(text.size() > 0);
	_btn_listen->setEnabled(_le_url->text().size() > 5);
	_combo_stream->setToolTip("");
}

void AbstractStream::url_text_changed(const QString& url){

	QString key = _stations.key(url.trimmed());

	if(! key.isEmpty() ) {

		int idx = _combo_stream->findText(key, Qt::MatchCaseSensitive);

		if(idx != -1) {

			_combo_stream->setCurrentIndex(idx);
			_btn_save->setEnabled(false);
			_btn_delete->setEnabled(true);

			_cur_station_idx = idx;
		}
	}

	// new adress
	else{

		_btn_delete->setEnabled(false);

		bool save_enabled =
				_combo_stream->currentText().size() > 0 &&
				_le_url->text().size() > 5 &&
				_cur_station_idx == -1;

		_btn_save->setEnabled(save_enabled);
		_btn_listen->setEnabled(url.size() > 5);

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

	_btn_listen->setEnabled(false);
	_btn_save->setEnabled(false);
	_btn_delete->setEnabled(false);
}

