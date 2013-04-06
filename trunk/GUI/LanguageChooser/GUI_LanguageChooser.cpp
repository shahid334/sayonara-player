#include "GUI/LanguageChooser/GUI_LanguageChooser.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/CSettingsStorage.h"
#include <QFile>
#include <QDir>
#include <QDebug>


GUI_LanguageChooser::GUI_LanguageChooser(QWidget *parent) :
    QDialog(parent)
{
    ui = new Ui::GUI_LanguageChooser();
    ui->setupUi(this);

    _map["en"] = "English";
    _map["de"] = "Deutsch";

    connect(ui->combo_lang, SIGNAL(currentIndexChanged(int)), this, SLOT(combo_changed(int)));
    connect(ui->btn_ok, SIGNAL(clicked()), this, SLOT(ok_clicked()));
}

GUI_LanguageChooser::~GUI_LanguageChooser(){
    delete ui;
}


void GUI_LanguageChooser::language_changed(){
    ui->retranslateUi(this);

}

void GUI_LanguageChooser::combo_changed(int idx){
    int cur_idx = ui->combo_lang->currentIndex();
    emit sig_language_changed(ui->combo_lang->itemData(cur_idx).toString());
}


void GUI_LanguageChooser::ok_clicked(){

    int cur_idx = ui->combo_lang->currentIndex();
    CSettingsStorage::getInstance()->setLanguage(ui->combo_lang->itemData(cur_idx).toString());

    this->close();
}


void GUI_LanguageChooser::showEvent(QShowEvent * event){

    QDir dir(Helper::getSharePath() + "translations/");
    QStringList filters;
    filters << "*.qm";
    QStringList files = dir.entryList(filters);

    ui->combo_lang->clear();

    QString lang_setting = CSettingsStorage::getInstance()->getLanguage();
    int tgt_idx = 0;
    int i=0;
    foreach(QString file, files){
        int idx = file.lastIndexOf(QDir::separator());
        QString name = file.right(file.size() - idx);

        name = name.left(name.size() - 3);

        QString two = name.right(2);
        QString title = _map.value(two);
        QString flag = Helper::getSharePath() + "/translations/" + two + ".png";

        if(title.size() > 0)
            ui->combo_lang->addItem(QIcon(flag), title, name);
        else
            ui->combo_lang->addItem(name, name);


        if(name.compare(lang_setting, Qt::CaseInsensitive) == 0) tgt_idx = i;
        i++;
    }

    ui->combo_lang->setCurrentIndex(tgt_idx);

    event->accept();
}
