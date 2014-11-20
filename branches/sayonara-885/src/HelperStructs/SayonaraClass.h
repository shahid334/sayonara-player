#ifndef SAYONARACLASS_H
#define SAYONARACLASS_H

#include <QWidget>
#include <QDialog>
#include <QMainWindow>
#include "Settings/Settings.h"


class SayonaraClass {

	protected:
		Settings* _settings;

	public:
		SayonaraClass()
		{
			_settings = Settings::getInstance();
		}

		virtual ~SayonaraClass(){

		}
};

class SayonaraDialog : public QDialog, protected SayonaraClass {

	Q_OBJECT

private slots:
	virtual void language_changed(){}
	virtual void skin_changed(){}

public:
	SayonaraDialog(QWidget* parent=0) :
		QDialog(parent),
		SayonaraClass()

	{
		REGISTER_LISTENER(Set::Player_Language, language_changed);
		REGISTER_LISTENER(Set::Player_Style, skin_changed);
	}

	virtual ~SayonaraDialog(){

	}

};

class SayonaraWidget : public QWidget, protected SayonaraClass {

	Q_OBJECT

private slots:
	virtual void language_changed(){}
	virtual void skin_changed(){}

public:
	SayonaraWidget(QWidget* parent=0) :
		QWidget(parent),
		SayonaraClass()

	{
		REGISTER_LISTENER(Set::Player_Language, language_changed);
		REGISTER_LISTENER(Set::Player_Style, skin_changed);
	}

	virtual ~SayonaraWidget(){

	}
};


class SayonaraMainWindow : public QMainWindow, protected SayonaraClass {

	Q_OBJECT

private slots:
	virtual void language_changed(){}
	virtual void skin_changed(){}

public:
	SayonaraMainWindow(QWidget* parent=0) :
		QMainWindow(parent),
		SayonaraClass()

	{
		REGISTER_LISTENER(Set::Player_Language, language_changed);
		REGISTER_LISTENER(Set::Player_Style, skin_changed);
	}

	virtual ~SayonaraMainWindow(){

	}
};



#endif // SAYONARACLASS_H
