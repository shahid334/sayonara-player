#include "GUI/ui_GUI_Target_Playlist_Dialog.h"
#include "GUI/TargetPlaylistDialog/GUI_Target_Playlist_Dialog.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/CSettingsStorage.h"

#include <QFileDialog>

GUI_Target_Playlist_Dialog::GUI_Target_Playlist_Dialog(QWidget *parent) :
    QDialog(parent)
{
    ui = new Ui_GUI_Target_Playlist_Dialog();
    ui->setupUi(this);

    connect(ui->btn_choose, SIGNAL(clicked()), this, SLOT(search_button_clicked()));
    connect(ui->btn_ok, SIGNAL(clicked()), this, SLOT(ok_button_clicked()));
}

GUI_Target_Playlist_Dialog::~GUI_Target_Playlist_Dialog(){

}


void GUI_Target_Playlist_Dialog::change_skin(bool dark){

}



void GUI_Target_Playlist_Dialog::search_button_clicked(){
    QString target_filename = QFileDialog::getSaveFileName(this,
                                                           "Choose target file",
                                                           CSettingsStorage::getInstance()->getLibraryPath(),
                                                           "*.m3u");

    if(!target_filename.endsWith("m3u", Qt::CaseInsensitive)) target_filename.append(".m3u");
    this->ui->le_path->setText(target_filename);
}


void GUI_Target_Playlist_Dialog::ok_button_clicked(){
    QString target_filename = ui->le_path->text();
    bool checked = ui->cb_relative->isChecked();

    if(target_filename.size() > 0){
        emit sig_target_chosen(target_filename, checked);
        close();
    }

}
