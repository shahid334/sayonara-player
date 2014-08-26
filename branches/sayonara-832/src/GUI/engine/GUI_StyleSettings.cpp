/* GUI_StyleSettings.cpp */

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



#include "GUI_StyleSettings.h"
#include "HelperStructs/Helper.h"
#include <QColorDialog>
#include <QMessageBox>


GUI_StyleSettings::GUI_StyleSettings(QWidget *parent) :
	QDialog(parent),
	Ui::GUI_Style()
{

	setupUi(this);

    _db = CDatabaseConnector::getInstance();


	btn_delete->setIcon(QIcon(Helper::getIconPath() + "delete.png"));
	btn_save->setIcon(QIcon(Helper::getIconPath() + "save.png"));
	btn_undo->setIcon(QIcon(Helper::getIconPath() + "undo.png"));

	connect(combo_styles, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_styles_changed(int)));
	connect(combo_styles, SIGNAL(editTextChanged(const QString&)), this, SLOT(combo_text_changed(const QString&)));
	connect(btn_col1, SIGNAL(clicked()), this, SLOT(col1_activated()));
	connect(btn_col2, SIGNAL(clicked()), this, SLOT(col2_activated()));
	connect(btn_col3, SIGNAL(clicked()), this, SLOT(col3_activated()));
	connect(btn_col4, SIGNAL(clicked()), this, SLOT(col4_activated()));
	connect(btn_save, SIGNAL(clicked()), this, SLOT(save_pressed()));
	connect(btn_delete, SIGNAL(clicked()), this, SLOT(del_pressed()));
	connect(btn_undo, SIGNAL(clicked()), this, SLOT(undo_pressed()));

	connect(sb_fading_steps_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	connect(sb_fading_steps_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	connect(sb_h_spacing_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	connect(sb_h_spacing_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	connect(sb_n_bins_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	connect(sb_rect_height_sp, SIGNAL(valueChanged(int)), this,SLOT( spin_box_changed(int)));
	connect(sb_rect_width_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	connect(sb_rect_height_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	connect(sb_v_spacing_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	connect(sb_v_spacing_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));

    set_sth_changed(false);

    init();
}

void GUI_StyleSettings::language_changed() {
	retranslateUi(this);
}

void GUI_StyleSettings::init() {
	combo_styles->clear();

    _styles.clear();
    _styles = _db->get_raw_color_styles();

    RawColorStyle style;

    for(int i=0; i<4; i++) {
        _colors[i] = QColor(0,0,0,0);
        style.col_list.colors << QColor(0,0,0,0);
    }

    _styles.push_front(style);

    foreach(RawColorStyle style, _styles) {
		combo_styles->addItem(style.col_list.name);
    }

	_cur_idx = combo_styles->currentIndex();
	_cur_text = combo_styles->currentText();
    _cur_style = _styles[_cur_idx];


}

QIcon col2Icon(QColor col) {

    QPixmap pm(18,18);
    pm.fill(col);

    QIcon icon(pm);
    return icon;
}

void GUI_StyleSettings::connect_spinboxes() {
	connect(sb_fading_steps_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	connect(sb_fading_steps_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	connect(sb_h_spacing_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	connect(sb_h_spacing_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	connect(sb_n_bins_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	connect(sb_rect_height_sp, SIGNAL(valueChanged(int)), this,SLOT( spin_box_changed(int)));
	connect(sb_rect_width_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	connect(sb_rect_height_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	connect(sb_v_spacing_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	connect(sb_v_spacing_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));

}


void GUI_StyleSettings::disconnect_spinboxes() {
	disconnect(sb_fading_steps_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	disconnect(sb_fading_steps_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	disconnect(sb_h_spacing_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	disconnect(sb_h_spacing_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	disconnect(sb_n_bins_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	disconnect(sb_rect_height_sp, SIGNAL(valueChanged(int)), this,SLOT( spin_box_changed(int)));
	disconnect(sb_rect_width_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	disconnect(sb_rect_height_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	disconnect(sb_v_spacing_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
	disconnect(sb_v_spacing_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));

}

void GUI_StyleSettings::combo_styles_changed(int idx) {

    int new_idx = idx;

    if(_sth_changed) {

        int ret = QMessageBox::warning(this,
                             tr("Warning"),
                             tr("There are some unsaved settings<br />Save now?"),
                             QMessageBox::Yes,
                             QMessageBox::No);

        if(ret == QMessageBox::Yes) {

            save_pressed();
            // save was not successful
            if(_cur_text.size() == 0) {

				disconnect(combo_styles, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_styles_changed(int)));
				combo_styles->setCurrentIndex(_cur_idx);
				connect(combo_styles, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_styles_changed(int)));
                return;
            }

            else {
                set_sth_changed(false);

				disconnect(combo_styles, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_styles_changed(int)));
				combo_styles->setCurrentIndex(new_idx);
				connect(combo_styles, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_styles_changed(int)));
            }
        }

        else {
            set_sth_changed(false);
        }
    }

    disconnect_spinboxes();

	_cur_idx = combo_styles->currentIndex();
	_cur_text = combo_styles->currentText();

	btn_delete->setDisabled(idx == 0);
	btn_undo->setDisabled(idx == 0);

    RawColorStyle style = _styles[idx];
	sb_fading_steps_lv->setValue(style.n_fading_steps_level);
	sb_fading_steps_sp->setValue(style.n_fading_steps_spectrum);
	sb_h_spacing_lv->setValue(style.hor_spacing_level);
	sb_h_spacing_sp->setValue(style.hor_spacing_spectrum);
	sb_v_spacing_lv->setValue(style.ver_spacing_level);
	sb_v_spacing_sp->setValue(style.ver_spacing_spectrum);
	sb_n_bins_sp->setValue(style.n_bins_spectrum);
	sb_rect_height_sp->setValue(style.rect_height_spectrum);
	sb_rect_width_lv->setValue(style.rect_width_level);
	sb_rect_height_lv->setValue(style.rect_height_level);

	cb_col3->setChecked(style.col_list.colors.size() > 2 && style.col_list.colors[2].isValid());
	cb_col4->setChecked(style.col_list.colors.size() > 3 && style.col_list.colors[3].isValid());

    QList<QColor> col_list = style.col_list.colors;
	btn_col1->setIcon(col2Icon(col_list[0]));
	btn_col2->setIcon(col2Icon(col_list[1]));

	if(col_list.size() > 2) btn_col3->setIcon(col2Icon(col_list[2]));
	else  btn_col3->setIcon(col2Icon(QColor(0, 0, 0, 0)));

	if(col_list.size() > 3) btn_col4->setIcon(col2Icon(col_list[3]));
	else  btn_col4->setIcon(col2Icon(QColor(0, 0, 0, 0)));

    _colors[0] = col_list[0];
    _colors[1] = col_list[1];
    _colors[2] = (col_list.size() > 2) ? col_list[2] : QColor(0,0,0,0);
    _colors[3] = (col_list.size() > 3) ? col_list[3] : QColor(0,0,0,0);

    connect_spinboxes();

	_cur_idx = combo_styles->currentIndex();
	_cur_text = combo_styles->currentText();
    _cur_style = _styles[_cur_idx];
    set_sth_changed(false);
}

void GUI_StyleSettings::save_pressed() {

    // we came from [0]
    if(_cur_idx == 0 && _cur_text.size() == 0) {
        QMessageBox::warning(this, tr("Error"), tr("Please specify a name"));
        return;
    }

    RawColorStyle style;
	style.n_fading_steps_level = sb_fading_steps_lv->value();
	style.n_fading_steps_spectrum = sb_fading_steps_sp->value();
	style.hor_spacing_level = sb_h_spacing_lv->value();
	style.hor_spacing_spectrum = sb_h_spacing_sp->value();
	style.ver_spacing_level = sb_v_spacing_lv->value();
	style.ver_spacing_spectrum = sb_v_spacing_sp->value();
	style.n_bins_spectrum = sb_n_bins_sp->value();
	style.rect_height_spectrum = sb_rect_height_sp->value();
	style.rect_width_level = sb_rect_width_lv->value();
	style.rect_height_level = sb_rect_height_lv->value();

    style.col_list.name = _cur_text;
    style.col_list.colors << _colors[0] << _colors[1];
	if(cb_col3->isChecked()) style.col_list.colors << _colors[2];
	if(cb_col4->isChecked()) style.col_list.colors << _colors[3];

    _db->update_raw_color_style(style);

    _styles[_cur_idx] = style;
    set_sth_changed(false);

	disconnect(combo_styles, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_styles_changed(int)));

    int cur_idx = _cur_idx;
    RawColorStyle tmp_style = _cur_style;
    init();
	combo_styles->setCurrentIndex(cur_idx);
    _cur_idx = cur_idx;
	_cur_text = combo_styles->currentText();

	connect(combo_styles, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_styles_changed(int)));
    combo_styles_changed(_cur_idx);

    _cur_style = tmp_style;

    emit sig_style_update();
}


void GUI_StyleSettings::col1_activated() {


		int cur_style = combo_styles->currentIndex();

        QColor col_new = QColorDialog::getColor(_styles[cur_style].col_list.colors[0], this);
		btn_col1->setIcon(col2Icon(col_new));
        _colors[0] = col_new;

        col_changed();

}

void GUI_StyleSettings::col2_activated() {
	int cur_style = combo_styles->currentIndex();

    QColor col_new = QColorDialog::getColor(_styles[cur_style].col_list.colors[1], this);
	btn_col2->setIcon(col2Icon(col_new));
    _colors[1] = col_new;

    col_changed();

}

void GUI_StyleSettings::col3_activated() {

	int cur_style = combo_styles->currentIndex();

    QColor col_old(255, 255, 255);
    if(_styles[cur_style].col_list.colors.size() > 2)col_old = _styles[cur_style].col_list.colors[2];
    QColor col_new = QColorDialog::getColor(col_old, this);

	btn_col3->setIcon(col2Icon(col_new));
    _colors[2] = col_new;

    col_changed();
}


void GUI_StyleSettings::col4_activated() {

	int cur_style = combo_styles->currentIndex();

    QColor col_old(255, 255, 255);
    if(_styles[cur_style].col_list.colors.size() > 3) col_old = _styles[cur_style].col_list.colors[3];

    QColor col_new = QColorDialog::getColor(col_old, this);

	btn_col4->setIcon(col2Icon(col_new));
    _colors[3] = col_new;

    col_changed();
}


void GUI_StyleSettings::del_pressed() {

	setWindowTitle(windowTitle().remove("*"));

	disconnect(combo_styles, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_styles_changed(int)));


    _db->delete_raw_color_style(_cur_text);
    init();
	connect(combo_styles, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_styles_changed(int)));

	_cur_idx = combo_styles->currentIndex();
	_cur_text = combo_styles->currentText();

    set_sth_changed(false);
    emit sig_style_update();


}

void GUI_StyleSettings::undo_pressed() {

    disconnect_spinboxes();

	sb_fading_steps_lv->setValue(_cur_style.n_fading_steps_level);
	sb_fading_steps_sp->setValue(_cur_style.n_fading_steps_spectrum);
	sb_h_spacing_lv->setValue(_cur_style.hor_spacing_level);
	sb_h_spacing_sp->setValue(_cur_style.hor_spacing_spectrum);
	sb_v_spacing_lv->setValue(_cur_style.ver_spacing_level);
	sb_v_spacing_sp->setValue(_cur_style.ver_spacing_spectrum);
	sb_n_bins_sp->setValue(_cur_style.n_bins_spectrum);
	sb_rect_height_sp->setValue(_cur_style.rect_height_spectrum);
	sb_rect_width_lv->setValue(_cur_style.rect_width_level);
	sb_rect_height_lv->setValue(_cur_style.rect_height_level);

	cb_col3->setChecked(_cur_style.col_list.colors.size() > 2 && _cur_style.col_list.colors[2].isValid());
	cb_col4->setChecked(_cur_style.col_list.colors.size() > 3 && _cur_style.col_list.colors[3].isValid());

    QList<QColor> col_list = _cur_style.col_list.colors;
	btn_col1->setIcon(col2Icon(col_list[0]));
	btn_col2->setIcon(col2Icon(col_list[1]));

	if(col_list.size() > 2) btn_col3->setIcon(col2Icon(col_list[2]));
	else  btn_col3->setIcon(col2Icon(QColor(0, 0, 0, 0)));

	if(col_list.size() > 3) btn_col4->setIcon(col2Icon(col_list[3]));
	else  btn_col4->setIcon(col2Icon(QColor(0, 0, 0, 0)));

    _colors[0] = col_list[0];
    _colors[1] = col_list[1];
    _colors[2] = (col_list.size() > 2) ? col_list[2] : QColor(0,0,0,0);
    _colors[3] = (col_list.size() > 3) ? col_list[3] : QColor(0,0,0,0);

    set_sth_changed(false);

    connect_spinboxes();
}




void GUI_StyleSettings::closeEvent(QCloseEvent * e) {
    if(_sth_changed) {
        int ret = QMessageBox::warning(this, tr("Warning"), tr("Save changes?"), QMessageBox::Yes, QMessageBox::No);
        if(ret == QMessageBox::Yes) {
            save_pressed();

        }

        e->accept();
    }
}

void GUI_StyleSettings::spin_box_changed(int v) {
    Q_UNUSED(v);
    set_sth_changed(true);
}

void GUI_StyleSettings::col_changed() {
    set_sth_changed(true);

}

void GUI_StyleSettings::combo_text_changed(const QString & str) {
	if(_cur_idx != combo_styles->currentIndex()) return;
	_cur_text = combo_styles->currentText();
}


void GUI_StyleSettings::set_sth_changed(bool b) {
	btn_undo->setEnabled(b);
    _sth_changed = b;
    if(b) {
		if(!windowTitle().endsWith("*")) {
			setWindowTitle(windowTitle() + "*");
        }
    }
    else
		setWindowTitle(windowTitle().remove("*"));
}


void GUI_StyleSettings::show(int idx) {
	if(isVisible()) return;

    set_sth_changed(false);
	showNormal();
    if(idx < _styles.size() - 1)
		combo_styles->setCurrentIndex(idx + 1);
    else
		combo_styles->setCurrentIndex(0);

}
