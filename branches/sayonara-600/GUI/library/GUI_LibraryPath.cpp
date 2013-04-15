#include "GUI/library/GUI_LibraryPath.h"
#include "HelperStructs/Helper.h"
#include <QIcon>
#include <QDebug>


GUI_LibraryPath::GUI_LibraryPath(QWidget *parent) :
    QWidget(parent)
{
    ui = new Ui::GUI_SetLibrary();
    ui->setupUi(this);


    QIcon import(Helper::getIconPath() + "/import.png" );
    this->ui->btn_setLibrary->setIcon(import);

    connect(ui->btn_setLibrary, SIGNAL(clicked()), this, SLOT(btn_clicked()));
}

void GUI_LibraryPath::language_changed() {
    ui->retranslateUi(this);
}


void GUI_LibraryPath::btn_clicked(){

    emit sig_library_path_set();

}

void GUI_LibraryPath::resizeEvent(QResizeEvent* e){


    e->accept();
}


