#ifndef GUI_SPEED_H
#define GUI_SPEED_H

#include "PlayerPlugin/PlayerPlugin.h"
#include "GUI/ui_GUI_Speed.h"

class GUI_Speed : public PlayerPlugin, private Ui::GUI_Speed
{
	Q_OBJECT

public:
	explicit GUI_Speed(QString name, QWidget *parent = 0);
		
signals:
	void sig_speed_changed(float);

private slots:
	void slider_changed(int);
	void active_changed(bool);

private:
	Ui::GUI_Speed* ui;

	
};

#endif // GUI_SPEED_H
