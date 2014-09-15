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
#include "HelperStructs/CSettingsStorage.h"
#include <QFile>
#include <QDir>

GUI_LanguageChooser::GUI_LanguageChooser(QWidget *parent) :
	QDialog(parent),
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


    _last_idx = -1;

	setModal(true);

	connect(btn_ok, SIGNAL(clicked()), this, SLOT(ok_clicked()));
}

GUI_LanguageChooser::~GUI_LanguageChooser() {

}


void GUI_LanguageChooser::language_changed(bool b) {

	disconnect(combo_lang, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_changed(int)));
	retranslateUi(this);
	connect(combo_lang, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_changed(int)));
    renew_combo();

}

void GUI_LanguageChooser::combo_changed(int idx) {
	int cur_idx = combo_lang->currentIndex();
    _last_idx = cur_idx;
	emit sig_language_changed(combo_lang->itemData(cur_idx).toString());


}


void GUI_LanguageChooser::ok_clicked() {

	int cur_idx = combo_lang->currentIndex();
	CSettingsStorage::getInstance()->setLanguage(combo_lang->itemData(cur_idx).toString());

	close();
}


void GUI_LanguageChooser::renew_combo() {

	disconnect(combo_lang, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_changed(int)));
    QDir dir(Helper::getSharePath() + "translations/");
    QStringList filters;
    filters << "*.qm";
    QStringList files = dir.entryList(filters);

	combo_lang->clear();

    QString lang_setting = CSettingsStorage::getInstance()->getLanguage();

    int tgt_idx = 0;
    int i=0;
    foreach(QString file, files) {
        int idx = file.lastIndexOf(QDir::separator());
        QString name = file.right(file.size() - idx);

        name = name.left(name.size() - 3);

        QString two = name.right(2);
        QString title = _map.value(two);
        QString flag = Helper::getSharePath() + "translations/icons/" + two + ".png";

        if(title.size() > 0)
			combo_lang->addItem(QIcon(flag), title, name);
        else
			combo_lang->addItem(name, name);


        if(name.compare(lang_setting, Qt::CaseInsensitive) == 0) tgt_idx = i;
        i++;
    }

    if(_last_idx == -1) _last_idx = tgt_idx;
    else tgt_idx = _last_idx;

	combo_lang->setCurrentIndex(tgt_idx);
	connect(combo_lang, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_changed(int)));
}

void GUI_LanguageChooser::showEvent(QShowEvent * event) {


    renew_combo();
    _last_idx = -1;


    event->accept();
}
