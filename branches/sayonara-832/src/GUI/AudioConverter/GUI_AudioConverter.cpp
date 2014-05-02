#include "GUI_AudioConverter.h"
#include "HelperStructs/Helper.h"
#include <QFileDialog>

GUI_AudioConverter::GUI_AudioConverter(QString name, QString action_text, QWidget *parent) :
	PlayerPlugin(name, action_text, parent)
{
	ui = new Ui::GUI_AudioConvert();
	ui->setupUi(this);

	_settings = CSettingsStorage::getInstance();

	connect(ui->rb_cbr, SIGNAL(toggled(bool)), this, SLOT(rb_cbr_toggled(bool)));
	connect(ui->rb_vbr, SIGNAL(toggled(bool)), this, SLOT(rb_vbr_toggled(bool)));
	connect(ui->cb_quality, SIGNAL(currentIndexChanged(int)), this, SLOT(quality_changed(int)));
	connect(ui->cb_active, SIGNAL(toggled(bool)), this, SLOT(cb_active_toggled(bool)));

	ui->rb_cbr->setChecked(true);
}


void GUI_AudioConverter::fill_cbr(){

	ui->cb_quality->clear();

	ui->cb_quality->addItem("64", LameBitrate_64);
	ui->cb_quality->addItem("128", LameBitrate_128);
	ui->cb_quality->addItem("192", LameBitrate_192);
	ui->cb_quality->addItem("256", LameBitrate_256);
	ui->cb_quality->addItem("320", LameBitrate_320);

	ui->cb_quality->setCurrentIndex(2);
}

void GUI_AudioConverter::fill_vbr(){
	ui->cb_quality->clear();

	ui->cb_quality->addItem(tr("0 (Best)"), LameBitrate_var_0);
	ui->cb_quality->addItem("1", LameBitrate_var_1);
	ui->cb_quality->addItem("2", LameBitrate_var_2);
	ui->cb_quality->addItem("3", LameBitrate_var_3);
	ui->cb_quality->addItem("4", LameBitrate_var_4);
	ui->cb_quality->addItem("5", LameBitrate_var_5);
	ui->cb_quality->addItem("6", LameBitrate_var_6);
	ui->cb_quality->addItem("7", LameBitrate_var_7);
	ui->cb_quality->addItem("8", LameBitrate_var_8);
	ui->cb_quality->addItem(tr("9 (Worst)"), LameBitrate_var_9);

	ui->cb_quality->setCurrentIndex(2);
}

void GUI_AudioConverter::rb_cbr_toggled(bool b){
	if(!b) return;
	fill_cbr();
}

void GUI_AudioConverter::rb_vbr_toggled(bool b){
	if(!b) return;
	fill_vbr();
}

void GUI_AudioConverter::cb_active_toggled(bool b){

	if(b) {
		QString dir = QFileDialog::getExistingDirectory(this, "Choose target directory", _settings->getConvertTgtPath());
		if(dir.size() > 0){
			_settings->setConvertTgtPath(dir);
			emit sig_active();
		}

		else {
			ui->cb_active->setChecked(false);
		}

	}

	else emit sig_inactive();
}

void GUI_AudioConverter::quality_changed(int index){
	LameBitrate q = (LameBitrate) ui->cb_quality->itemData(index).toInt();
	qDebug() << "Quality: " << q;
	_settings->setConvertQuality(q);
}

QAction* GUI_AudioConverter::getAction(){
	PlayerPlugin::calc_action(this->getVisName());
	return _pp_action;
}
