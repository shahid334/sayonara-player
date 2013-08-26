#include "GUI_StyleSettings.h"
#include "HelperStructs/Helper.h"
#include <QColorDialog>
#include <QMessageBox>


GUI_StyleSettings::GUI_StyleSettings(QWidget *parent) :
    QDialog(parent)
{

    ui = new Ui::GUI_Style();
    ui->setupUi(this);

    _db = CDatabaseConnector::getInstance();


    this->ui->btn_delete->setIcon(QIcon(Helper::getIconPath() + "delete.png"));
    this->ui->btn_save->setIcon(QIcon(Helper::getIconPath() + "save.png"));
    this->ui->btn_undo->setIcon(QIcon(Helper::getIconPath() + "undo.png"));

    connect(ui->combo_styles, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_styles_changed(int)));
    connect(ui->combo_styles, SIGNAL(editTextChanged(const QString&)), this, SLOT(combo_text_changed(const QString&)));
    connect(ui->btn_col1, SIGNAL(clicked()), this, SLOT(col1_activated()));
    connect(ui->btn_col2, SIGNAL(clicked()), this, SLOT(col2_activated()));
    connect(ui->btn_col3, SIGNAL(clicked()), this, SLOT(col3_activated()));
    connect(ui->btn_col4, SIGNAL(clicked()), this, SLOT(col4_activated()));
    connect(ui->btn_save, SIGNAL(clicked()), this, SLOT(save_pressed()));
    connect(ui->btn_delete, SIGNAL(clicked()), this, SLOT(del_pressed()));
    connect(ui->btn_undo, SIGNAL(clicked()), this, SLOT(undo_pressed()));

    connect(ui->sb_fading_steps_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
    connect(ui->sb_fading_steps_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
    connect(ui->sb_h_spacing_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
    connect(ui->sb_h_spacing_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
    connect(ui->sb_n_bins_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
    connect(ui->sb_rect_height_sp, SIGNAL(valueChanged(int)), this,SLOT( spin_box_changed(int)));
    connect(ui->sb_rect_width_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
    connect(ui->sb_v_spacing_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
    connect(ui->sb_v_spacing_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));

    set_sth_changed(false);

    init();
}

void GUI_StyleSettings::language_changed(){
    this->ui->retranslateUi(this);
}

void GUI_StyleSettings::init(){
    ui->combo_styles->clear();

    _styles.clear();
    _styles = _db->get_raw_color_styles();

    RawColorStyle style;

    for(int i=0; i<4; i++){
        _colors[i] = QColor(0,0,0,0);
        style.col_list.colors << QColor(0,0,0,0);
    }

    _styles.push_front(style);

    foreach(RawColorStyle style, _styles){
        ui->combo_styles->addItem(style.col_list.name);
    }

    _cur_idx = ui->combo_styles->currentIndex();
    _cur_text = ui->combo_styles->currentText();
    _cur_style = _styles[_cur_idx];


}

QIcon col2Icon(QColor col){

    QPixmap pm(18,18);
    pm.fill(col);

    QIcon icon(pm);
    return icon;
}

void GUI_StyleSettings::connect_spinboxes(){
    connect(ui->sb_fading_steps_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
    connect(ui->sb_fading_steps_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
    connect(ui->sb_h_spacing_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
    connect(ui->sb_h_spacing_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
    connect(ui->sb_n_bins_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
    connect(ui->sb_rect_height_sp, SIGNAL(valueChanged(int)), this,SLOT( spin_box_changed(int)));
    connect(ui->sb_rect_width_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
    connect(ui->sb_v_spacing_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
    connect(ui->sb_v_spacing_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));

}


void GUI_StyleSettings::disconnect_spinboxes(){
    disconnect(ui->sb_fading_steps_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
    disconnect(ui->sb_fading_steps_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
    disconnect(ui->sb_h_spacing_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
    disconnect(ui->sb_h_spacing_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
    disconnect(ui->sb_n_bins_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
    disconnect(ui->sb_rect_height_sp, SIGNAL(valueChanged(int)), this,SLOT( spin_box_changed(int)));
    disconnect(ui->sb_rect_width_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
    disconnect(ui->sb_v_spacing_lv, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));
    disconnect(ui->sb_v_spacing_sp, SIGNAL(valueChanged(int)), this, SLOT(spin_box_changed(int)));

}

void GUI_StyleSettings::combo_styles_changed(int idx){

    int new_idx = idx;

    if(_sth_changed){

        int ret = QMessageBox::warning(this,
                             tr("Warning"),
                             tr("There are some unsaved settings<br />Save now?"),
                             QMessageBox::Yes,
                             QMessageBox::No);

        if(ret == QMessageBox::Yes){

            save_pressed();
            // save was not successful
            if(_cur_text.size() == 0) {

                disconnect(ui->combo_styles, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_styles_changed(int)));
                ui->combo_styles->setCurrentIndex(_cur_idx);
                connect(ui->combo_styles, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_styles_changed(int)));
                return;
            }

            else {
                set_sth_changed(false);

                disconnect(ui->combo_styles, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_styles_changed(int)));
                ui->combo_styles->setCurrentIndex(new_idx);
                connect(ui->combo_styles, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_styles_changed(int)));
            }
        }

        else {
            set_sth_changed(false);
        }
    }

    disconnect_spinboxes();

    _cur_idx = ui->combo_styles->currentIndex();
    _cur_text = ui->combo_styles->currentText();

    ui->btn_delete->setDisabled(idx == 0);
    ui->btn_undo->setDisabled(idx == 0);

    RawColorStyle style = _styles[idx];
    ui->sb_fading_steps_lv->setValue(style.n_fading_steps_level);
    ui->sb_fading_steps_sp->setValue(style.n_fading_steps_spectrum);
    ui->sb_h_spacing_lv->setValue(style.hor_spacing_level);
    ui->sb_h_spacing_sp->setValue(style.hor_spacing_spectrum);
    ui->sb_v_spacing_lv->setValue(style.ver_spacing_level);
    ui->sb_v_spacing_sp->setValue(style.ver_spacing_spectrum);
    ui->sb_n_bins_sp->setValue(style.n_bins_spectrum);
    ui->sb_rect_height_sp->setValue(style.rect_height_spectrum);
    ui->sb_rect_width_lv->setValue(style.rect_width_level);

    ui->cb_col3->setChecked(style.col_list.colors.size() > 2 && style.col_list.colors[2].isValid());
    ui->cb_col4->setChecked(style.col_list.colors.size() > 3 && style.col_list.colors[3].isValid());

    QList<QColor> col_list = style.col_list.colors;
    ui->btn_col1->setIcon(col2Icon(col_list[0]));
    ui->btn_col2->setIcon(col2Icon(col_list[1]));

    if(col_list.size() > 2) ui->btn_col3->setIcon(col2Icon(col_list[2]));
    else  ui->btn_col3->setIcon(col2Icon(QColor(0, 0, 0, 0)));

    if(col_list.size() > 3) ui->btn_col4->setIcon(col2Icon(col_list[3]));
    else  ui->btn_col4->setIcon(col2Icon(QColor(0, 0, 0, 0)));

    _colors[0] = col_list[0];
    _colors[1] = col_list[1];
    _colors[2] = (col_list.size() > 2) ? col_list[2] : QColor(0,0,0,0);
    _colors[3] = (col_list.size() > 3) ? col_list[3] : QColor(0,0,0,0);

    connect_spinboxes();

    _cur_idx = ui->combo_styles->currentIndex();
    _cur_text = ui->combo_styles->currentText();
    _cur_style = _styles[_cur_idx];
    set_sth_changed(false);
}

void GUI_StyleSettings::save_pressed(){

    // we came from [0]
    if(_cur_idx == 0 && _cur_text.size() == 0) {
        QMessageBox::warning(this, tr("Error"), tr("Please specify a name"));
        return;
    }

    RawColorStyle style;
    style.n_fading_steps_level = ui->sb_fading_steps_lv->value();
    style.n_fading_steps_spectrum = ui->sb_fading_steps_sp->value();
    style.hor_spacing_level = ui->sb_h_spacing_lv->value();
    style.hor_spacing_spectrum = ui->sb_h_spacing_sp->value();
    style.ver_spacing_level = ui->sb_v_spacing_lv->value();
    style.ver_spacing_spectrum = ui->sb_v_spacing_sp->value();
    style.n_bins_spectrum = ui->sb_n_bins_sp->value();
    style.rect_height_spectrum = ui->sb_rect_height_sp->value();
    style.rect_width_level = ui->sb_rect_width_lv->value();

    style.col_list.name = _cur_text;
    style.col_list.colors << _colors[0] << _colors[1];
    if(ui->cb_col3->isChecked()) style.col_list.colors << _colors[2];
    if(ui->cb_col4->isChecked()) style.col_list.colors << _colors[3];

    _db->update_raw_color_style(style);

    _styles[_cur_idx] = style;
    set_sth_changed(false);

    disconnect(ui->combo_styles, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_styles_changed(int)));

    int cur_idx = _cur_idx;
    RawColorStyle tmp_style = _cur_style;
    init();
    ui->combo_styles->setCurrentIndex(cur_idx);
    _cur_idx = cur_idx;
    _cur_text = ui->combo_styles->currentText();

    connect(ui->combo_styles, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_styles_changed(int)));
    combo_styles_changed(_cur_idx);

    _cur_style = tmp_style;

    emit sig_style_update();
}


void GUI_StyleSettings::col1_activated(){


        int cur_style = this->ui->combo_styles->currentIndex();

        QColor col_new = QColorDialog::getColor(_styles[cur_style].col_list.colors[0], this);
        ui->btn_col1->setIcon(col2Icon(col_new));
        _colors[0] = col_new;

        col_changed();

}

void GUI_StyleSettings::col2_activated(){
    int cur_style = this->ui->combo_styles->currentIndex();

    QColor col_new = QColorDialog::getColor(_styles[cur_style].col_list.colors[1], this);
    ui->btn_col2->setIcon(col2Icon(col_new));
    _colors[1] = col_new;

    col_changed();

}

void GUI_StyleSettings::col3_activated(){

    int cur_style = this->ui->combo_styles->currentIndex();

    QColor col_old(255, 255, 255);
    if(_styles[cur_style].col_list.colors.size() > 2)col_old = _styles[cur_style].col_list.colors[2];
    QColor col_new = QColorDialog::getColor(col_old, this);

    ui->btn_col3->setIcon(col2Icon(col_new));
    _colors[2] = col_new;

    col_changed();
}


void GUI_StyleSettings::col4_activated(){

    int cur_style = this->ui->combo_styles->currentIndex();

    QColor col_old(255, 255, 255);
    if(_styles[cur_style].col_list.colors.size() > 3) col_old = _styles[cur_style].col_list.colors[3];

    QColor col_new = QColorDialog::getColor(col_old, this);

    ui->btn_col4->setIcon(col2Icon(col_new));
    _colors[3] = col_new;

    col_changed();
}


void GUI_StyleSettings::del_pressed(){

    this->setWindowTitle(windowTitle().remove("*"));

    disconnect(ui->combo_styles, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_styles_changed(int)));


    _db->delete_raw_color_style(_cur_text);
    init();
    connect(ui->combo_styles, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_styles_changed(int)));

    _cur_idx = ui->combo_styles->currentIndex();
    _cur_text = ui->combo_styles->currentText();

    set_sth_changed(false);
    emit sig_style_update();


}

void GUI_StyleSettings::undo_pressed(){

    disconnect_spinboxes();

    ui->sb_fading_steps_lv->setValue(_cur_style.n_fading_steps_level);
    ui->sb_fading_steps_sp->setValue(_cur_style.n_fading_steps_spectrum);
    ui->sb_h_spacing_lv->setValue(_cur_style.hor_spacing_level);
    ui->sb_h_spacing_sp->setValue(_cur_style.hor_spacing_spectrum);
    ui->sb_v_spacing_lv->setValue(_cur_style.ver_spacing_level);
    ui->sb_v_spacing_sp->setValue(_cur_style.ver_spacing_spectrum);
    ui->sb_n_bins_sp->setValue(_cur_style.n_bins_spectrum);
    ui->sb_rect_height_sp->setValue(_cur_style.rect_height_spectrum);
    ui->sb_rect_width_lv->setValue(_cur_style.rect_width_level);

    ui->cb_col3->setChecked(_cur_style.col_list.colors.size() > 2 && _cur_style.col_list.colors[2].isValid());
    ui->cb_col4->setChecked(_cur_style.col_list.colors.size() > 3 && _cur_style.col_list.colors[3].isValid());

    QList<QColor> col_list = _cur_style.col_list.colors;
    ui->btn_col1->setIcon(col2Icon(col_list[0]));
    ui->btn_col2->setIcon(col2Icon(col_list[1]));

    if(col_list.size() > 2) ui->btn_col3->setIcon(col2Icon(col_list[2]));
    else  ui->btn_col3->setIcon(col2Icon(QColor(0, 0, 0, 0)));

    if(col_list.size() > 3) ui->btn_col4->setIcon(col2Icon(col_list[3]));
    else  ui->btn_col4->setIcon(col2Icon(QColor(0, 0, 0, 0)));

    _colors[0] = col_list[0];
    _colors[1] = col_list[1];
    _colors[2] = (col_list.size() > 2) ? col_list[2] : QColor(0,0,0,0);
    _colors[3] = (col_list.size() > 3) ? col_list[3] : QColor(0,0,0,0);

    set_sth_changed(false);

    connect_spinboxes();
}




void GUI_StyleSettings::closeEvent(QCloseEvent * e){
    if(_sth_changed){
        int ret = QMessageBox::warning(this, tr("Warning"), tr("Save changes?"), QMessageBox::Yes, QMessageBox::No);
        if(ret == QMessageBox::Yes) {
            save_pressed();

        }

        e->accept();
    }
}

void GUI_StyleSettings::spin_box_changed(int v){
    Q_UNUSED(v);
    set_sth_changed(true);
}

void GUI_StyleSettings::col_changed(){
    set_sth_changed(true);

}

void GUI_StyleSettings::combo_text_changed(const QString & str){
    if(_cur_idx != ui->combo_styles->currentIndex()) return;
    _cur_text = ui->combo_styles->currentText();
}


void GUI_StyleSettings::set_sth_changed(bool b){
    ui->btn_undo->setEnabled(b);
    _sth_changed = b;
    if(b)
        this->setWindowTitle(windowTitle() + "*");
    else
        this->setWindowTitle(windowTitle().remove("*"));
}


void GUI_StyleSettings::show(int idx){
    if(this->isVisible()) return;

    set_sth_changed(false);
    this->showNormal();
    if(idx < _styles.size() - 1)
        ui->combo_styles->setCurrentIndex(idx + 1);
    else
        ui->combo_styles->setCurrentIndex(0);

}
