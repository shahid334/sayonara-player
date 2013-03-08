#ifndef GUI_STARTUP_DIALOG_H
#define GUI_STARTUP_DIALOG_H

#include <QDialog>
#include "GUI/ui_GUI_Startup_Dialog.h"

class GUI_Startup_Dialog : public QDialog, private Ui::GUI_StartupDialog
{
    Q_OBJECT
public:
    GUI_Startup_Dialog(QWidget *parent = 0);
    virtual ~GUI_Startup_Dialog();
    
signals:
    
public slots:

private slots:
    void cb_toggled(bool);
    void ok_clicked();

private:
    Ui::GUI_StartupDialog* ui;
    
};

#endif // GUI_STARTUP_DIALOG_H
