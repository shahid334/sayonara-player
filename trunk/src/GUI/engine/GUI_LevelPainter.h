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

    float _level_l;
    float _level_r;

    QColor _white;
    QColor _red;
    QColor _blue;
    QColor _green;
    QColor _yellow;
    QColor _black;

    QColor _col_r;
    QColor _col_l;


    
};

#endif // GUI_LEVELPAINTER_H
