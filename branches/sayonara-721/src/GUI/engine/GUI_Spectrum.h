#ifndef GUI_SPECTRUM_H
#define GUI_SPECTRUM_H

#include "PlayerPlugin/PlayerPlugin.h"
#include "GUI/ui_GUI_Spectrum.h"
#include <QCloseEvent>
#include <QShowEvent>
#include <QPaintEvent>
#include <QAction>
#include <QColor>
#include <QList>

class GUI_Spectrum : public PlayerPlugin, private Ui::GUI_Spectrum
{
    Q_OBJECT
public:
    explicit GUI_Spectrum(QString name, QString action_text, QWidget *parent=0);

    static QString getVisName(){ return tr("S&pectrum"); }
    virtual QAction* getAction();

signals:
    void sig_show(bool);
    void closeEvent();

protected:

    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    void paintEvent(QPaintEvent* e);

public slots:
    void set_spectrum(QList<float>&);

private:
    Ui::GUI_Spectrum* ui;


    QColor _white;
    QColor _red, _red_dark;
    QColor _blue;
    QColor _green, _green_dark;
    QColor _yellow, _yellow_dark;
    QColor _black;

    QList<float> _spec;




};

#endif // GUI_SPECTRUM_H
