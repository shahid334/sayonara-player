#ifndef GUI_PODCASTS_H
#define GUI_PODCASTS_H

#include <QDockWidget>
#include "ui_GUI_Podcasts.h"
#include "PlayerPlugin/PlayerPlugin.h"
#include <QWidget>
#include <QMap>
#include <QCloseEvent>

class GUI_Podcasts : public PlayerPlugin, private Ui::GUI_Podcasts
{
    Q_OBJECT

signals:
    void sig_play_podcast(const QString&, const QString&);
    void sig_close_event();

private slots:
    void listen_clicked();
    void combo_index_changed(int idx);
    void combo_text_changed(const QString& text);
    void url_text_changed(const QString& text);
    void delete_clicked();
    void save_clicked();



public:
    GUI_Podcasts(QString name, QString action_text, QChar shortcut, QWidget *parent = 0);
    virtual ~GUI_Podcasts();



private:
    Ui::GUI_Podcasts* ui;

    void init_gui();
    QMap<QString, QString> _podcasts;
    int _cur_podcast;
    QString _cur_podcast_name;
    QString _cur_podcast_adress;

    void setup_podcasts(const QMap<QString, QString>&);

protected:
    void 	closeEvent ( QCloseEvent * event );

};

#endif // GUI_PODCASTS_H
