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
#include <vector>
#include <HelperStructs/Helper.h>
#include <HelperStructs/MetaData.h>

using namespace std;

GUI_Library_windowed::GUI_Library_windowed(QWidget* parent) : QWidget(parent) {
	this->ui = new Ui::Library_windowed();
	this->ui->setupUi(this);

	this->_model = new LibraryItemModelTracks();
	this->ui->tb_title->setModel(this->_model);

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
