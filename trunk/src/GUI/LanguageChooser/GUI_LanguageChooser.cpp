/* GUI_LanguageChooser.cpp */

/* Copyright (C) 2011-2014  Lucio Carreras
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


#include "GUI/LanguageChooser/GUI_LanguageChooser.h"
#include "HelperStructs/Helper.h"

#include <QFile>
#include <QDir>

GUI_LanguageChooser::GUI_LanguageChooser(QWidget *parent) :
	SayonaraDialog(parent),
	Ui::GUI_LanguageChooser()
{
	setupUi(this);

    _map["br"] = QString::fromUtf8("Português (Brasil)");
    _map["cs"] = QString::fromUtf8("Český");
    _map["de"] = "Deutsch";
    _map["en"] = "English";
    _map["es"] = QString::fromUtf8("Español");
    _map["mx"] = QString::fromUtf8("Español (México)");
    _map["fr"] = "Francais";
    _map["hu"] = "Magyar";
    _map["it"] = "Italiano";
    _map["pt"] = QString::fromUtf8("Português");
    _map["ro"] = QString::fromUtf8("Limba română");
    _map["ru"] = QString::fromUtf8("Русский");
    _map["ua"] = QString::fromUtf8("Українська");

	setModal(true);

	connect(btn_ok, SIGNAL(clicked()), this, SLOT(ok_clicked()));
}

GUI_LanguageChooser::~GUI_LanguageChooser() {

}

void GUI_LanguageChooser::language_changed(){
	retranslateUi(this);
}


void GUI_LanguageChooser::ok_clicked() {

	int cur_idx = combo_lang->currentIndex();
	QString cur_language = combo_lang->itemData(cur_idx).toString();

	_settings->set(Set::Player_Language, cur_language);

	close();
}


void GUI_LanguageChooser::renew_combo() {


	QString lang_setting = _settings->get(Set::Player_Language);
	qDebug() << "Language setting = " << lang_setting;
    QDir dir(Helper::getSharePath() + "translations/");

	QStringList filters;
		filters << "*.qm";
    QStringList files = dir.entryList(filters);

	combo_lang->clear();

    int i=0;
    foreach(QString file, files) {

		QString filename, dirname;
		Helper::split_filename(file, dirname, filename);

		filename = filename.left(filename.size() - 3);

		QString two = filename.right(2);
        QString title = _map.value(two);
        QString flag = Helper::getSharePath() + "translations/icons/" + two + ".png";

		if(title.size() > 0){
			combo_lang->addItem(QIcon(flag), title, filename);
		}

		else{
			combo_lang->addItem(filename, filename);
		}

		if(filename.compare(lang_setting, Qt::CaseInsensitive) == 0){
			combo_lang->setCurrentIndex(i);
		}

		i++;
    }
}

void GUI_LanguageChooser::showEvent(QShowEvent* e) {

    renew_combo();
	QDialog::showEvent(e);
}
