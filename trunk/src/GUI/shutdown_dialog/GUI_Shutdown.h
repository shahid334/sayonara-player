#ifndef GUI_SHUTDOWN_H
#define GUI_SHUTDOWN_H

#include "HelperStructs/SayonaraClass.h"
#include "GUI/ui_GUI_Shutdown.h"


class GUI_Shutdown : public SayonaraDialog, protected Ui::GUI_Shutdown
{
	Q_OBJECT

signals:
	void sig_closed();

private slots:

	void ok_clicked();
	void cancel_clicked();
	void rb_after_finished_clicked(bool b);
	void rb_after_minutes_clicked(bool b);

public:
	GUI_Shutdown(QWidget* parent=0);
};

#endif // GUI_SHUTDOWN_H
