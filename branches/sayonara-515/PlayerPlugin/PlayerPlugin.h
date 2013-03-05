#ifndef PLAYERPLUGIN_H
#define PLAYERPLUGIN_H

#include <QWidget>
#include <QAction>

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

    QSize       _pp_size;
    bool        _pp_is_shown;

protected:

    void setSize(QSize size);



public:

    QSize getSize();
    QString getName();

    void setShown(bool b);
    QAction* getAction();

    
};

#endif // PLAYERPLUGIN_H
