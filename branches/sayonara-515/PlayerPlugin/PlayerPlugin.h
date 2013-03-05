#ifndef PLAYERPLUGIN_H
#define PLAYERPLUGIN_H

#include <QWidget>
#include <QAction>
#include <QCloseEvent>

class PlayerPlugin : public QWidget
{
    Q_OBJECT





public:
    PlayerPlugin(QString name, QString action_text, QChar shortcut, QWidget *parent = 0);
    virtual ~PlayerPlugin();

    
signals:

    void sig_action_triggered(PlayerPlugin*, bool);

    
public slots:

private slots:
    void action_triggered(bool);

private:
    QString     _pp_name;
    QAction*    _pp_action;

    bool        _pp_is_shown;


protected:

    void setSize(QSize size);
    void closeEvent(QCloseEvent *);



public:

    QSize getSize();
    QString getName();

    void setShown(bool b);
    QAction* getAction();

    
};

#endif // PLAYERPLUGIN_H
