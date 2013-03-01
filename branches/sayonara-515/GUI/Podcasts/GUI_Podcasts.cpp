#include "GUI/Podcasts/GUI_Podcasts.h"
#include "ui_GUI_Podcasts.h"

#include "HelperStructs/Helper.h"
#include "HelperStructs/Style.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QDockWidget>
#include <QIcon>
#include <QPixmap>
#include <QMap>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>


GUI_Podcasts::GUI_Podcasts(QWidget *parent) :
    QWidget(parent)
{

    this->ui = new Ui::GUI_Podcasts();
    this->ui->setupUi(this);

    init_gui();

    _cur_podcast = -1;

    QMap<QString, QString> data;
    CDatabaseConnector::getInstance()->getAllPodcasts(data);
    if(data.size() > 0)
        setup_podcasts(data);

    this->ui->btn_listen->setIcon(QIcon(Helper::getIconPath() + "play.png"));

    this->connect(this->ui->btn_listen, SIGNAL(clicked()), this, SLOT(listen_clicked()));
    this->connect(this->ui->btn_save, SIGNAL(clicked()), this, SLOT(save_clicked()));
    this->connect(this->ui->btn_delete, SIGNAL(clicked()), this, SLOT(delete_clicked()));
    this->connect(this->ui->combo_podcasts, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_index_changed(int)));
    this->connect(this->ui->combo_podcasts, SIGNAL(editTextChanged(const QString&)), this, SLOT(combo_text_changed(const QString&)));

    this->connect(this->ui->le_url, SIGNAL(textEdited(const QString&)), this, SLOT(url_text_changed(const QString&)));

    hide();
}


GUI_Podcasts::~GUI_Podcasts() {
    // TODO Auto-generated destructor stub
}



void GUI_Podcasts::listen_clicked(){

    QString url;
    QString name;

    if(_cur_podcast == -1){
        url = this->ui->le_url->text();
        name = "Podcast";
    }

    else{
        qDebug() << "Emit: " << _cur_podcast_name << ": " << _cur_podcast_adress;
        url = _cur_podcast_adress;
        name = _cur_podcast_name;
    }

    if(url.size() > 5){

        emit sig_play_podcast(url.trimmed(), name);
    }
}


void GUI_Podcasts::setup_podcasts(const QMap<QString, QString>& podcasts){

    _podcasts = podcasts;
    if(podcasts.size() > 0){
        _cur_podcast = -1;
    }

    this->ui->combo_podcasts->clear();

    _cur_podcast_adress = "";
    _cur_podcast_name = "";
    _cur_podcast = 0;

    _podcasts[""] = "";

    for(QMap<QString, QString>::iterator it = _podcasts.begin(); it != _podcasts.end(); it++){
        this->ui->combo_podcasts->addItem(it.key(), it.value());
    }

    this->ui->btn_listen->setEnabled(false);
    this->ui->btn_save->setEnabled(false);
    this->ui->btn_delete->setEnabled(false);
}


void GUI_Podcasts::init_gui(){
    this->ui->btn_delete->setIcon(QIcon(Helper::getIconPath() + "delete.png"));
    this->ui->btn_save->setIcon(QIcon(Helper::getIconPath() + "save.png"));

    this->ui->lab_icon->setPixmap(QPixmap(Helper::getIconPath() + "podcast.png"));
}


void GUI_Podcasts::combo_index_changed(int idx){

    _cur_podcast = idx;
    _cur_podcast_name = this->ui->combo_podcasts->itemText(_cur_podcast);

    QString adress = _podcasts[_cur_podcast_name];
    if(adress.size() > 0){
        _cur_podcast_adress = adress;
        this->ui->le_url->setText(_cur_podcast_adress);
    }

    if(idx == 0){
        this->ui->le_url->setText("");
    }


    this->ui->btn_delete->setEnabled(idx > 0);
    this->ui->btn_save->setEnabled(false);
    this->ui->btn_listen->setEnabled(this->ui->le_url->text().size() > 5);
    this->ui->combo_podcasts->setToolTip(_cur_podcast_adress);
}


void GUI_Podcasts::combo_text_changed(const QString& text){
    _cur_podcast = -1;

    this->ui->btn_delete->setEnabled(false);
    this->ui->btn_save->setEnabled((text.size() > 0));
    this->ui->btn_listen->setEnabled(this->ui->le_url->text().size() > 5);
    this->ui->combo_podcasts->setToolTip("");
}

void GUI_Podcasts::url_text_changed(const QString& text){

    QString key = _podcasts.key(text);

    if(! key.isEmpty() ){

        int idx = this->ui->combo_podcasts->findText(key, Qt::MatchCaseSensitive);
        if(idx != -1){
            this->ui->combo_podcasts->setCurrentIndex(idx);
            _cur_podcast = idx;
            this->ui->btn_save->setEnabled(false);
            this->ui->btn_delete->setEnabled(true);
        }
    }

    // new adress
    else{

        this->ui->btn_delete->setEnabled(false);

        bool save_enabled =
                this->ui->combo_podcasts->currentText().size() > 0 &&
                this->ui->le_url->text().size() > 5 &&
                _cur_podcast == -1;

        this->ui->btn_save->setEnabled(save_enabled);
        this->ui->btn_listen->setEnabled(text.size() > 5);
        if(_cur_podcast != -1){
            _cur_podcast = -1;
            this->ui->combo_podcasts->setEditText("new");
            _cur_podcast = -1;
        }
    }

}


void GUI_Podcasts::delete_clicked(){
    if(_cur_podcast == -1) return;

    CDatabaseConnector* db = CDatabaseConnector::getInstance();
    QMessageBox msgBox;
    msgBox.setText("Really wanna delete" + _cur_podcast_name + "?" );
    msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    int ret = msgBox.exec();
    if(ret == QMessageBox::Yes){
        if(db->deletePodcast(_cur_podcast_name)){
            qDebug() << _cur_podcast_name << "successfully deleted";
            QMap<QString, QString> map;
            if(db->getAllPodcasts(map)){
                setup_podcasts(map);
            }
        }
    }

    _cur_podcast = -1;
}


void GUI_Podcasts::save_clicked(){
    CDatabaseConnector* db = CDatabaseConnector::getInstance();
    QString name = this->ui->combo_podcasts->currentText();
    QString url = this->ui->le_url->text();

    bool success = false;
    if(name.size() > 0 && url.size() > 0){
        success = db->addPodcast(name, url);
    }

    if(success){
        QMap<QString, QString> map;
        if(db->getAllPodcasts(map)){
            setup_podcasts(map);
        }
    }


    _cur_podcast = -1;
    this->ui->le_url->setText(url);
    url_text_changed(url);
}

void GUI_Podcasts::closeEvent ( QCloseEvent * event ){
    event->ignore();
    hide();
    close();
    emit sig_close_event();
}

