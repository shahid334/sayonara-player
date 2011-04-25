/*
 * GUI_Library_windowed.cpp
 *
 *  Created on: Apr 24, 2011
 *      Author: luke
 */

#include "GUI/Library/GUI_Library_windowed.h"
#include "GUI/Library/LibraryItemModelTracks.h"
#include "ui_GUI_Library_windowed.h"
#include <QDebug>
#include <QPoint>
#include <QMouseEvent>
#include <vector>
#include <HelperStructs/Helper.h>
#include <HelperStructs/MetaData.h>

using namespace std;

GUI_Library_windowed::GUI_Library_windowed(QWidget* parent) : QWidget(parent) {
	this->ui = new Ui::Library_windowed();
	this->ui->setupUi(this);

	this->_model = new LibraryItemModelTracks();
	this->ui->tb_title->setModel(this->_model);

	this->ui->gridLayout->setRowStretch(1, 2);
	this->ui->gridLayout->setRowStretch(2, 3);
	connect(this->ui->tb_title, SIGNAL(	pressed ( const QModelIndex & )), this, SLOT(track_pressed(const QModelIndex&)));

}

GUI_Library_windowed::~GUI_Library_windowed() {
	// TODO Auto-generated destructor stub
}


void GUI_Library_windowed::fill_library_tracks(vector<MetaData>& v_metadata){
	this->_model->removeRows(0, this->_model->rowCount());
	this->_model->insertRows(0, v_metadata.size());

	qDebug() << "inserted " << v_metadata.size() << " rows";

	for(uint i=0; i<v_metadata.size(); i++){

		QModelIndex idx = _model->index(i, 0);

		MetaData md = v_metadata.at(i);


		QStringList list;
		list.push_back(md.title);
		list.push_back(md.artist);
		list.push_back(md.album);

		int min, sec;
		Helper::cvtSecs2MinAndSecs(md.length_ms/1000, &min, &sec);
		QString length = QString::fromStdString(Helper::cvtNum2String(min, 2)) + ":" + QString::fromStdString(Helper::cvtNum2String(sec, 2));
		list.push_back(length);
		list.push_back(QString::number(md.year));

		this->_model->setData(idx, list, Qt::EditRole);
	}


}


void GUI_Library_windowed::resizeEvent(QResizeEvent* e){
		QSize tmpSize = this->ui->tb_title->size();
		int width = tmpSize.width();
		if(width > 700){
			this->ui->tb_title->setColumnWidth(0, width * 0.35);
			this->ui->tb_title->setColumnWidth(1, width * 0.23);
			this->ui->tb_title->setColumnWidth(2, width * 0.23);
			this->ui->tb_title->setColumnWidth(3, width * 0.08);
			this->ui->tb_title->setColumnWidth(4, width * 0.08);
			this->ui->tb_title->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		}

		else {
			this->ui->tb_title->setColumnWidth(0, width * 0.5);
			this->ui->tb_title->setColumnWidth(1, width * 0.25);
			this->ui->tb_title->setColumnWidth(2, width * 0.25);
			this->ui->tb_title->setColumnWidth(3, 80);
			this->ui->tb_title->setColumnWidth(4, 80);
			this->ui->tb_title->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);


		}

}




void GUI_Library_windowed::track_pressed(const QModelIndex& idx){

	qDebug() << "Mouse pressed at " << idx.row();
	QDrag* drag = new QDrag(this);
	QMimeData* mime = new QMimeData();

	QModelIndexList list = this->ui->tb_title->selectionModel()->selectedRows(0);
	qDebug() << list.size() << " items selcted";

	QString title;
	QString artist;
	QString album;
	QString length;

	for(int i=0; i<list.size(); i++){
		int row = list.at(i).row();



		title += this->_model->data(_model->index(row, 0), Qt::DisplayRole).toString() + "\n";
		artist += this->_model->data(_model->index(row, 1), Qt::DisplayRole).toString() + "\n";
		album += this->_model->data(_model->index(row, 2), Qt::DisplayRole).toString() + "\n";
		length += this->_model->data(_model->index(row, 3), Qt::DisplayRole).toString() + "\n";


	}




	mime->setProperty("title", title);
	mime->setProperty("artist", artist);
	mime->setProperty("album", album);
	mime->setProperty("length", length);

	drag->setMimeData(mime);

	Qt::DropAction dropAction = drag->exec();

}
