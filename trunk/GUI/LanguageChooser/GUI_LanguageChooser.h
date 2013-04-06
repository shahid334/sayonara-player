#ifndef GUI_LANGUAGECHOOSER_H
#define GUI_LANGUAGECHOOSER_H

#include <QDialog>
#include <QMap>
#include <QShowEvent>

#include "../ui_GUI_LanguageChooser.h"

class GUI_LanguageChooser : public QDialog, private Ui::GUI_LanguageChooser
{
    Q_OBJECT

public:

    explicit GUI_LanguageChooser(QWidget *parent = 0);
    virtual ~GUI_LanguageChooser();

signals:
    void sig_language_changed(QString);
    
public slots:
    void language_changed(bool b);

private slots:
    void combo_changed(int);
    void ok_clicked();


private:
    Ui::GUI_LanguageChooser* ui;
    QMap<QString, QString> _map;
    int _last_idx;
    void renew_combo();

protected:
    void showEvent(QShowEvent *);
    
};

#endif // GUI_LANGUAGECHOOSER_H
