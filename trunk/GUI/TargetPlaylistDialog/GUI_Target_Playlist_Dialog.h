#ifndef GUI_TARGET_PLAYLIST_DIALOG_H
#define GUI_TARGET_PLAYLIST_DIALOG_H

#include <QDialog>
#include <ui_GUI_Target_Playlist_Dialog.h>

class GUI_Target_Playlist_Dialog : public QDialog, private Ui_GUI_Target_Playlist_Dialog
{
    Q_OBJECT
public:
    GUI_Target_Playlist_Dialog(QWidget *parent = 0);
    virtual ~GUI_Target_Playlist_Dialog();
    void change_skin(bool dark);
    
signals:
    void sig_target_chosen(const QString&, bool);
    
private slots:
    void search_button_clicked();
    void ok_button_clicked();

private:
    Ui_GUI_Target_Playlist_Dialog* ui;
    
};

#endif // GUI_TARGET_PLAYLIST_DIALOG_H
