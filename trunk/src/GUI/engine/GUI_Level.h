#ifndef GUI_LEVEL_H
#define GUI_LEVEL_H

#include "PlayerPlugin/PlayerPlugin.h"
#include "GUI/ui_GUI_Level.h"
#include <QString>
#include <QCloseEvent>
#include <QShowEvent>
#include <QTimer>
#include <QList>
#include <QPair>

class GUI_Level : public PlayerPlugin, private Ui::GUI_Level
{
    Q_OBJECT
public:
    GUI_Level(QString name, QString action_name, QWidget* parent=0);
    virtual ~GUI_Level();

    static QString getVisName(){ return tr("Le&vel"); }
    virtual QAction* getAction();
    
signals:
    void sig_show(bool);
    void closeEvent();

protected:

    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    
public slots:
    void set_level(float, float);

private slots:
    void timeout();

private:
    float* _levels_l;
    float* _levels_r;
    int    _cur_idx;
    int    _buffer_size;

    QList<QPair<float, float> > _queue;

    Ui::GUI_Level* ui;

    QTimer* _timer;
    bool _queue_it;

    
};

#endif // GUI_LEVEL_H
