#include "GUI/alternate_covers/GUI_AlternateCovInternet.h"
#include <QDir>
#include <QString>


GUI_AlternateCovInternet::GUI_AlternateCovInternet(QWidget *parent) :
    QWidget(parent)
{

    this->ui = new Ui::GUI_AlternateCovInternet();
    this->ui->setupUi(this);

    _class_name = "GUI_AlternateCovInternet";
    _cov_lookup = new CoverLookup(_class_name);

    connect(this->ui->btn_search, SIGNAL(clicked()), this, SLOT(search_button_pressed()));
    connect(this->ui->tv_images, SIGNAL(pressed(const QModelIndex& )), this, SLOT(cover_pressed(const QModelIndex& )));
    connect(this->_cov_lookup, SIGNAL(sig_multi_covers_found(QString, int)), this, SLOT(covers_there(QString, int)));
    connect(this->_watcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(tmp_folder_changed(const QString&)));
    connect(this->_watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(tmp_folder_changed(const QString&)));
}

// Internet
void GUI_AlternateCovInternet::init(QString searchstring){

    cleanup();
    this->ui->le_search->setText(searchstring);
    this->ui->btn_search->setText("Search");

}


// INternet
void GUI_AlternateCovInternet::covers_there(QString classname, int n_covers){

    if(classname != _class_name) return;

    _filelist.clear();)

    QDir dir(_tmp_dir);
    QStringList entrylist;

    QStringList filters;
        filters << "*.jpg";
        filters << "*.png";
        filters << "*.gif";

    dir.setFilter(QDir::Files);
    dir.setNameFilters(filters);

    entrylist = dir.entryList();

    foreach (QString f, entrylist)
        _filelist << dir.absoluteFilePath(f);

    update_model();

    ui->pb_progress->setVisible(false);
    ui->btn_search->setText("Search");
}

// Internet
void GUI_AlternateCovInternet::tmp_folder_changed(const QString& directory){

    _filelist.clear();

    QDir dir(directory);
    QStringList entrylist;
    QStringList filters;
        filters << "*.jpg";
        filters << "*.png";
        filters << "*.gif";

    dir.setFilter(QDir::Files);
    dir.setNameFilters(filters);

    entrylist = dir.entryList();

    foreach (QString f, entrylist)
        _filelist << dir.absoluteFilePath(f);


    update_model();

    ui->pb_progress->setTextVisible(false);
    ui->pb_progress->setVisible(true);
    ui->pb_progress->setValue(_filelist.size() * 10);

    if(ui->pb_progress->value() == 100)
        ui->pb_progress->setVisible(false);
}

// Internet ->
void GUI_AlternateCovInternet::search_button_pressed(){

    _cur_idx = -1;
    _cov_lookup->terminate_thread();
    remove_old_files();

    if(ui->btn_search->text().compare("Stop") == 0){

        ui->btn_search->setText("Search");
        ui->pb_progress->setVisible(false);
        return;
    }

    QString searchstring = this->ui->le_search->text();

    ui->btn_search->setText("Stop");
    ui->pb_progress->setValue(0);
    ui->pb_progress->setTextVisible(true);
    ui->pb_progress->setVisible(true);

    _cov_lookup->search_images_by_searchstring(searchstring, 10, _search_for_album);
}


void GUI_AlternateCovInternet::cleanup(){

     _cov_lookup->terminate_thread();

     // cleanup local
     // cleanup internet

    _model->removeRows(0, _model->rowCount());
    _filelist.clear();

    ui->pb_progress->setTextVisible(false);
    ui->pb_progress->setVisible(false);
    ui->btn_search->setText("Search");
}


