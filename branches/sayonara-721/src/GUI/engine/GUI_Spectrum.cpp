#include "GUI_Spectrum.h"
#include <QPainter>
#include <QList>
#include <QDebug>
#include <cmath>
void insertColorOfRect(int bin, int n_bins, QList<float> & borders, QList<QColor>& colors, QMap<int, QColor>& map){
	
	QColor col;

	float x = (bin * 1.0f) / n_bins;
    int i=0;
	int r, g, b;

    if(bin == 0){
        map[bin] = colors[0];
        return;
    }

    while(x > borders[i]){
        i++;
    }

	float dx = (borders[i] - borders[i-1]);

    float dy = colors[i].red() - colors[i-1].red();
    r = (int) (( dy * (x-borders[i-1]) ) / dx + colors[i-1].red());

	dy = colors[i].green() - colors[i-1].green();
    g = (int) (( dy * (x-borders[i-1])) / dx + colors[i-1].green());

    dy = colors[i].blue() - colors[i-1].blue();
    b = (int) ((dy * (x-borders[i-1])) / dx + colors[i-1].blue());

    qDebug() << "Old gr: " << colors[i-1].green() << ", New gr: " << colors[i].green() << ", " << g << ":: dx: " << dx << ", dy: " << dy << ", x: " << x;



	col.setRed(r);
    col.setGreen(g);
	col.setBlue(b);
    col.setAlpha(colors[0].alpha());

    qDebug() << col;

	map[bin] = col;
}

const int border_y = 2;
const int border_x = 2;
const int h_rect = 5;
int n_rects;



GUI_Spectrum::GUI_Spectrum(QString name, QString action_text, QWidget *parent) :
    PlayerPlugin(name, action_text, parent)
{
    qDebug() << "Spectrum start";
    ui = new Ui::GUI_Spectrum();
    ui->setupUi(this);

    n_rects = this->height() / (h_rect + border_y);
    qDebug() << "n_rects: " << n_rects;

    QList<float> borders;
    /*borders << 0  << 0.4 << 0.7 << 0.9 << 1.0;
    QList<QColor> colors_active;
    colors_active << QColor(0, 216, 0)  << QColor(216, 216, 0) << QColor(216, 216, 0) << QColor(216, 0, 0) << QColor(216, 0, 0);
    QList<QColor> colors_inactive;
    colors_inactive << QColor(0, 25, 0, 96) << QColor(25, 25, 0, 96) << QColor(25, 25, 0, 96) << QColor(25, 0, 0, 96) << QColor(25, 0, 0, 96);*/

    borders << 0  << 0.5  << 0.9 << 1.0;
    QList<QColor> colors_active;
    colors_active << QColor(0, 216, 0)  << QColor(216, 216, 0) << QColor(216, 0, 0) << QColor(216, 0, 0);
    QList<QColor> colors_inactive;
    colors_inactive << QColor(0, 25, 0, 96) << QColor(25, 25, 0, 96) << QColor(25, 0, 0, 96) << QColor(25, 0, 0, 96);

    for(int i=0; i<n_rects; i++){

        insertColorOfRect(i, n_rects, borders, colors_active, _map_col_active);
        insertColorOfRect(i, n_rects, borders, colors_inactive, _map_col_inactive);

    }

}

void
GUI_Spectrum::set_spectrum(QList<float>& lst){

    _spec = lst;
    this->update();
}


void
GUI_Spectrum::paintEvent(QPaintEvent *e){
     QPainter painter(this);

    int x=10;
    int ninety = (_spec.size() * 500) / 1000;
    if(ninety == 0) return;

    int w_bin = ((this->width()) / ninety) - border_x;
    float widget_height = (float) this->height();

    for(int i=0; i<ninety; i++){

        float f = (_spec[i] + 80.0f) / 80.0f;

        // if this is one bar, how tall would it be?
        int h =  f * widget_height;

        // how many colored rectangles would fit into this bar?
        int colored_rects = h / (h_rect + border_y);

        // we start from bottom with painting
        int y = widget_height - h_rect;


        for(int r=0; r<n_rects; r++){

            QColor col;

            if( r < colored_rects){
                col = _map_col_active[r];
            }

            else{
                 col = _map_col_inactive[r];
            }

            QRect rect(x, y, w_bin, h_rect);
            painter.fillRect(rect, col);

            y -= (h_rect + border_y);
        }

        x += w_bin + border_x;
    }
}


QAction* GUI_Spectrum::getAction(){
    PlayerPlugin::calc_action(this->getVisName());
    return _pp_action;
}


void GUI_Spectrum::showEvent(QShowEvent * e){
    Q_UNUSED(e);
    emit sig_show(true);
}

void GUI_Spectrum::closeEvent(QCloseEvent *e){
    PlayerPlugin::closeEvent(e);
    emit sig_show(false);
}
