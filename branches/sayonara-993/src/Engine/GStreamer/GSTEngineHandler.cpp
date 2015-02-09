/* GSTEngineHandler.cpp */

/* Copyright (C) 2014  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY{} without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <qplugin.h>

#include "Engine/GStreamer/GSTEngineHandler.h"
#include "Engine/SoundPluginLoader.h"
#include "HelperStructs/Helper.h"
#include "Engine/GStreamer/GSTPlaybackEngine.h"
#include "Engine/GStreamer/GSTConvertEngine.h"


GSTEngineHandler::GSTEngineHandler(QObject* parent) : Engine() {
	_cur_engine = 0;

	_engines.push_back(new GSTPlaybackEngine());
	_engines.push_back(new GSTConvertEngine());

	psl_change_engine(PLAYBACK_ENGINE);
}


GSTEngineHandler::~GSTEngineHandler() {
	
}

void GSTEngineHandler::init() {

}


void GSTEngineHandler::start_convert() {

	stop();

	if( QString::compare(_cur_engine->getName(), CONVERT_ENGINE ) != 0) {
		psl_change_engine(CONVERT_ENGINE);
    }
}

void GSTEngineHandler::end_convert() {

	stop();

	qDebug() << "Engine end convert";
	if( QString::compare(_cur_engine->getName(), PLAYBACK_ENGINE ) != 0) {
		qDebug() << "Change to playback engine";
		psl_change_engine(PLAYBACK_ENGINE);
	}
}

void GSTEngineHandler::psl_set_speed(float f) {
	if(!_cur_engine) return;
	_cur_engine->psl_set_speed(f);
}

void GSTEngineHandler::fill_engines(const QVector<Engine*>& engines) {
	_engines = engines;

	psl_change_engine(PLAYBACK_ENGINE);
}


void GSTEngineHandler::play() {
	if(!_cur_engine) return;
	_cur_engine->play();
}


void GSTEngineHandler::stop() {

	if(!_cur_engine) return;
	_cur_engine->stop();
}

void GSTEngineHandler::pause() {
	if(!_cur_engine) return;
	_cur_engine->pause();
}



void GSTEngineHandler::jump_abs_s(quint32 where) {
	if(!_cur_engine) return;
	_cur_engine->jump_abs_s(where);
}

void GSTEngineHandler::jump_abs_ms(quint64 where) {
	if(!_cur_engine) return;
	_cur_engine->jump_abs_ms(where);
}

void GSTEngineHandler::jump_rel(quint32 where) {
	if(!_cur_engine) return;
	_cur_engine->jump_rel(where);
}

void GSTEngineHandler::jump_rel_ms(qint64 where){
    if(!_cur_engine) return;
    _cur_engine->jump_rel_ms(where);
}

void GSTEngineHandler::change_track(const MetaData& md, bool start_play) {
	if(!_cur_engine) return;
	_cur_engine->change_track(md, start_play);
}

void GSTEngineHandler::change_track(const QString& str, bool start_play) {
	if(!_cur_engine) return;
	_cur_engine->change_track(str, start_play);
}

void GSTEngineHandler::eq_changed(int band, int value) {
	if(!_cur_engine) return;
	_cur_engine->eq_changed(band, value);
}


void GSTEngineHandler::record_button_toggled(bool b) {
	if(!_cur_engine) return;
	_cur_engine->record_button_toggled(b);
}


void GSTEngineHandler::psl_new_stream_session() {
	if(!_cur_engine) return;
	_cur_engine->psl_new_stream_session();
}

void GSTEngineHandler::sl_md_changed(const MetaData& v) {
	emit sig_md_changed(v);
}

void GSTEngineHandler::sl_pos_changed_ms(quint64 v) {
	emit sig_pos_changed_ms(v);
}

void GSTEngineHandler::sl_pos_changed_s(quint32 v) {
	emit sig_pos_changed_s(v);
}

void GSTEngineHandler::sl_track_finished() {
	emit sig_track_finished();
}

void GSTEngineHandler::sl_scrobble(const MetaData& md) {
	emit sig_scrobble(md);
}

void GSTEngineHandler::sl_level(float l, float r) {
	emit sig_level(l, r);
}

void GSTEngineHandler::sl_spectrum(QList<float>& lst) {
	emit sig_spectrum(lst);
}


bool GSTEngineHandler::configure_connections(Engine* old_engine, Engine* new_engine) {

	if(!old_engine && !new_engine) return false;
	if(old_engine == new_engine) return false;

	if(old_engine) {
		disconnect(old_engine, SIGNAL(sig_md_changed(const MetaData&)), this, SLOT(sl_md_changed(const MetaData&)));
		disconnect(old_engine, SIGNAL(sig_pos_changed_ms(quint64)), this, SLOT(sl_pos_changed_ms(quint64)));
		disconnect(old_engine, SIGNAL(sig_pos_changed_s(quint32)), this, SLOT(sl_pos_changed_s(quint32)));
		disconnect(old_engine, SIGNAL(sig_track_finished()), this, SLOT(sl_track_finished()));
		disconnect(old_engine, SIGNAL(sig_scrobble(const MetaData&)), this, SLOT(sl_scrobble(const MetaData&)));
		disconnect(old_engine, SIGNAL(sig_level(float, float)), this, SLOT(sl_level(float, float)));
		disconnect(old_engine, SIGNAL(sig_spectrum(QList<float>&)), this, SLOT(sl_spectrum(QList<float>&)));
		disconnect(old_engine, SIGNAL(sig_data(uchar*, quint64)), this, SLOT(new_data(uchar*, quint64)));
	}

	if(new_engine) {
		connect(new_engine, SIGNAL(sig_md_changed(const MetaData&)), this, SLOT(sl_md_changed(const MetaData&)));
		connect(new_engine, SIGNAL(sig_pos_changed_ms(quint64)), this, SLOT(sl_pos_changed_ms(quint64)));
		connect(new_engine, SIGNAL(sig_pos_changed_s(quint32)), this, SLOT(sl_pos_changed_s(quint32)));
		connect(new_engine, SIGNAL(sig_track_finished()), this, SLOT(sl_track_finished()));
		connect(new_engine, SIGNAL(sig_scrobble(const MetaData&)), this, SLOT(sl_scrobble(const MetaData&)));
		connect(new_engine, SIGNAL(sig_level(float, float)), this, SLOT(sl_level(float, float)));
		connect(new_engine, SIGNAL(sig_spectrum(QList<float>&)), this, SLOT(sl_spectrum(QList<float>&)));
		connect(new_engine, SIGNAL(sig_data(uchar*, quint64)), this, SLOT(new_data(uchar*, quint64)));
	}

	return true;
}


void GSTEngineHandler::psl_change_engine(QString name) {
	Engine* new_engine=NULL;

	foreach(Engine* engine, _engines) {
		if(engine && name.compare(engine->getName(), Qt::CaseInsensitive) == 0) {
			new_engine = engine;
			break;
		}		
	}

	configure_connections(_cur_engine, new_engine);
	_cur_engine = new_engine;

}

Q_EXPORT_PLUGIN2(sayonara_gstreamer, GSTEngineHandler)
