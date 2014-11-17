#ifndef GUI_BROADCASTSETUP_H
#define GUI_BROADCASTSETUP_H

#include <QDialog>
#include "GUI/ui_GUI_BroadcastSetup.h"
#include "Settings/Settings.h"

class GUI_BroadcastSetup : public QDialog, private Ui::GUI_BroadcastSetup
{
	Q_OBJECT
public:
	explicit GUI_BroadcastSetup(QWidget *parent = 0);

signals:

	void sig_port_changed(int);
	void sig_active_changed(bool);
	void sig_prompt_changed(bool);

private slots:

	void accept();
	void reject();


public slots:
	virtual void show();

private:
	Settings* _settings;

};

#endif // GUI_BROADCASTSETUP_H
