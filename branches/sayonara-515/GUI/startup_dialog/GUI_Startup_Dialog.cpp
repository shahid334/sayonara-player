#include "GUI/startup_dialog/GUI_Startup_Dialog.h"
#include "GUI/ui_GUI_Startup_Dialog.h"
#include "HelperStructs/CSettingsStorage.h"

GUI_Startup_Dialog::GUI_Startup_Dialog(QWidget *parent) :
    QDialog(parent)
{

    CSettingsStorage* set = CSettingsStorage::getInstance();
    ui = new Ui::GUI_StartupDialog;
    ui->setupUi(this);

    ui->cb_load_pl_on_startup->setChecked(set->getLoadPlaylist());
    ui->cb_load_last_track->setChecked(set->getLoadLastTrack());
    ui->cb_remember_time->setChecked(set->getRememberTime());
    ui->cb_start_playing->setChecked(set->getStartPlaying());

    cb_toggled(true);

    connect(ui->cb_load_pl_on_startup, SIGNAL(toggled(bool)), this, SLOT(cb_toggled(bool)));
    connect(ui->cb_load_last_track, SIGNAL(toggled(bool)), this, SLOT(cb_toggled(bool)));
    connect(ui->cb_remember_time, SIGNAL(toggled(bool)), this, SLOT(cb_toggled(bool)));
    connect(ui->cb_start_playing, SIGNAL(toggled(bool)), this, SLOT(cb_toggled(bool)));
    connect(ui->btn_ok, SIGNAL(clicked()), this, SLOT(ok_clicked()));

}

GUI_Startup_Dialog::~GUI_Startup_Dialog(){

}


void GUI_Startup_Dialog::cb_toggled(bool b){
    Q_UNUSED(b);

    bool cb_load_pl_checked = ui->cb_load_pl_on_startup->isChecked();

    ui->cb_load_last_track->setEnabled(cb_load_pl_checked);
    ui->cb_remember_time->setEnabled(cb_load_pl_checked);
    ui->cb_start_playing->setEnabled(cb_load_pl_checked);


    bool cb_load_last_track_checked = ui->cb_load_last_track->isChecked() && ui->cb_load_last_track->isEnabled();
    ui->cb_remember_time->setEnabled(cb_load_last_track_checked);

}

void GUI_Startup_Dialog::ok_clicked(){

    CSettingsStorage* set = CSettingsStorage::getInstance();
    set->setLoadPlaylist(ui->cb_load_pl_on_startup->isChecked());
    set->setLoadLastTrack( ui->cb_load_last_track->isChecked() && ui->cb_load_last_track->isEnabled());
    set->setRememberTime(ui->cb_remember_time->isChecked() && ui->cb_remember_time->isEnabled());
    set->setStartPlaying(ui->cb_start_playing->isChecked() && ui->cb_start_playing->isEnabled());
    close();
}



