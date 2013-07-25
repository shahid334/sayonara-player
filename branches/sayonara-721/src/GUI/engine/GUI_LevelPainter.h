#ifndef GUI_LEVELPAINTER_H
#define GUI_LEVELPAINTER_H

#include "PlayerPlugin/PlayerPlugin.h"
#include "GUI/ui_GUI_LevelPainter.h"
#include <QCloseEvent>
#include <QShowEvent>
#include <QPaintEvent>
#include <QAction>
#include <QColor>

class GUI_LevelPainter : public PlayerPlugin, private Ui::GUI_LevelPainter
{
    Q_OBJECT
public:
    explicit GUI_LevelPainter(QString name, QString action_text, QWidget *parent=0);

    static QString getVisName(){ return tr("Le&vel"); }
    virtual QAction* getAction();

signals:
    void sig_show(bool);
    void closeEvent();

protected:

    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    void paintEvent(QPaintEvent* e);

public slots:
    void set_level(float, float);

private:
    Ui::GUI_LevelPainter* ui;

    float _level_l, _level_l_old;
    float _level_r, _level_r_old;

    QColor _white;
    QColor _red, _red_dark;
    QColor _blue;
    QColor _green, _green_dark;
    QColor _yellow, _yellow_dark;
    QColor _black;

    QColor _col_r;
    QColor _col_l;

    int get_last_bright_light(float level);


    
};

#endif // GUI_LEVELPAINTER_H
