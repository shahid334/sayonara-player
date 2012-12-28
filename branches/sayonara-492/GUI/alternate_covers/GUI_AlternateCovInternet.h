#ifndef GUI_ALTERNATECOVINTERNET_H
#define GUI_ALTERNATECOVINTERNET_H

#include <QWidget>
#include <ui_GUI_AlternateCovInternet.h>
#include "CoverLookup/CoverLookup.h"


class GUI_AlternateCovInternet : public QWidget, private Ui::GUI_AlternateCovInternet
{
    Q_OBJECT
public:
    explicit GUI_AlternateCovInternet(QWidget *parent = 0);

    void init(QString searchstring);

    void covers_there(QString classname, int n_covers);
    void tmp_folder_changed(const QString& directory);
    void search_button_pressed();
    void cleanup();
    
signals:
    
public slots:

private:

    Ui::GUI_AlternateCovInternet* ui;
    CoverLookup* _cov_lookup;

    
};

#endif // GUI_ALTERNATECOVINTERNET_H
