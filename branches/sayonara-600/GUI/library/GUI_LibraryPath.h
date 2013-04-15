#ifndef GUI_LIBRARYPATH_H
#define GUI_LIBRARYPATH_H

#include <QWidget>
#include <QResizeEvent>
#include "GUI/ui_GUI_LibraryPath.h"


class GUI_LibraryPath : public QWidget, private Ui::GUI_SetLibrary
{
    Q_OBJECT
public:
    explicit GUI_LibraryPath(QWidget *parent = 0);
    
signals:
    void sig_library_path_set();
    
public slots:
    void language_changed();

private slots:
    void btn_clicked();

private:
    Ui::GUI_SetLibrary* ui;

protected:
    void resizeEvent(QResizeEvent* e);
    
};

#endif // GUI_LIBRARYPATH_H
