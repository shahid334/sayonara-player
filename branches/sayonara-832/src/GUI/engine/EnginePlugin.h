#ifndef ENGINEPLUGIN_H
#define ENGINEPLUGIN_H

#include "PlayerPlugin/PlayerPlugin.h"

#include <QTimer>
#include <QEvent>
#include <QShowEvent>

#include <QPushButton>

class EnginePlugin : public PlayerPlugin {

	Q_OBJECT

signals:
	void sig_show(bool);
	void sig_right_clicked(int);

protected:

	QPushButton* _btn_config;
	QPushButton* _btn_prev;
	QPushButton* _btn_next;
	QPushButton* _btn_close;


protected slots:
	virtual void config_clicked()=0;
	virtual void next_clicked()=0;
	virtual void prev_clicked()=0;



public:
	EnginePlugin(QString name, QWidget* parent=0, QWidget* child=0) :
		PlayerPlugin(name, parent) {

	}

	virtual ~EnginePlugin(){

		disconnect(_btn_config, SIGNAL(clicked()), this, SLOT(config_clicked()));
		disconnect(_btn_prev, SIGNAL(clicked()), this, SLOT(prev_clicked()));
		disconnect(_btn_next, SIGNAL(clicked()), this, SLOT(next_clicked()));
		disconnect(_btn_close, SIGNAL(clicked()), this, SLOT(close()));

		delete _btn_config;
		delete _btn_prev;
		delete _btn_next;
		delete _btn_close;
	}

	virtual void enterEvent(QEvent* e){
		PlayerPlugin::enterEvent(e);
		_btn_config->show();
		_btn_prev->show();
		_btn_next->show();
		_btn_close->show();

	}

	virtual void leaveEvent(QEvent* e){
		PlayerPlugin::leaveEvent(e);
		_btn_config->hide();
		_btn_prev->hide();
		_btn_next->hide();
		_btn_close->hide();
	}


};

#endif // ENGINEPLUGIN_H


