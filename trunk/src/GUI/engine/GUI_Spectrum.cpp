#include "GUI_Spectrum.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/CSettingsStorage.h"
#include <QPainter>
#include <QList>
#include <QDebug>
#include <cmath>

float log_lu[1100];

void insertColorOfRect(int bin, int n_bins, QList<float> & borders, QList<QColor>& colors, QMap<int, QColor>& map){
	
	QColor col;

    float x = (bin * 1.0f) / n_bins;
    int i=0;
    int r, g, b, a;

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

    dy = colors[i].alpha() - colors[i-1].alpha();
    a = (int) ((dy * (x-borders[i-1])) / dx + colors[i-1].alpha());

    col.setRed(r);
    col.setGreen(g);
	col.setBlue(b);
    col.setAlpha(a);

	map[bin] = col;
}

const int border_y = 1;
const int border_x = 2;
const int h_rect = 3;
const int n_fading_steps = 20;
int n_rects;



GUI_Spectrum::GUI_Spectrum(QString name, QString action_text, QWidget *parent) :
    PlayerPlugin(name, action_text, parent)
{

    _cur_col = CSettingsStorage::getInstance()->getSpectrumStyle();
    ui = new Ui::GUI_Spectrum();
    ui->setupUi(this);

    n_rects = this->height() / (h_rect + border_y);
    qDebug() << "n_rects: " << n_rects;

    QList<float> borders_4, borders_3, borders_2;
    borders_4 << 0  << 0.33  << 0.66 << 1.0;
    borders_3 << 0  << 0.50  << 1.0;
    borders_2 << 0  << 1.0;
    QList< QList<QColor> > colors_active;

    QList<QColor> fancy_4;
    fancy_4 << QColor(0, 216, 0)  << QColor(216, 216, 0) << QColor(216, 0, 0) << QColor(216, 0, 0);

    QList<QColor> bw_4;
    bw_4 << QColor(128, 128, 128)  << QColor(0, 0, 0) << QColor(192, 192, 192) << QColor(255,255,255);

    QList<QColor> orange_4;
    orange_4 << QColor(128, 128, 128,0)  << QColor(128, 128, 128) << QColor(243, 132, 26) << QColor(243, 132, 26);

    QList<QColor> green_4;
    green_4 << QColor(128, 128, 128,0)  << QColor(128, 128, 128) << QColor(0, 255, 0) << QColor(0, 255, 0);

    QList<QColor> blue_4;
    blue_4 << QColor(128, 128, 128,0)  << QColor(128, 128, 128) << QColor(26, 132, 243);

   /* QList<QColor> fancy_3;
    fancy_3 << QColor(0, 216, 0)  << QColor(216, 216, 0) << QColor(216, 0, 0);



    QList<QColor> orange_3;
    orange_3 << QColor(0, 0, 0)  << QColor(128, 128, 128) << QColor(243, 132, 26);

    QList<QColor> green_3;
    green_3 << QColor(128, 128, 128)  << QColor(128, 128, 128) << QColor(26, 255, 132);

    QList<QColor> blue_3;
    blue_3 << QColor(128, 128, 128)  << QColor(128, 128, 128) << QColor(26, 132, 243);*/

    QList<QColor> bw_3;
    bw_3 << QColor(26, 132, 243).darker(1000)  << QColor(128, 128, 128) << QColor(255,255,255);

    QList<QColor> fancy_2;
    fancy_2 << QColor(0, 216, 0)  << QColor(216, 0, 0);

    QList<QColor> bw_2;
    bw_2 << QColor(0, 0, 0)  << QColor(255,255,255);

    QList<QColor> orange_2;
    orange_2 << QColor(96, 96, 96)  << QColor(243, 132, 26);

    QList<QColor> green_2;
    green_2 << QColor(128, 128, 128)  << QColor(0, 255, 0);

    QList<QColor> blue_2;
    blue_2 << QColor(128, 128, 128)  << QColor(26, 132, 243);

    colors_active << fancy_4 << /*fancy_3 << fancy_2*/
                  //<< green_4 << /*green_3 <<*/ green_2
                  /*<< blue_4 << blue_3 <<*/ blue_2
                  /*<< orange_4 << orange_3*/ << orange_2
                  /*<< bw_4 */<< bw_3 /*<< bw_2*/;

    QList<QColor> colors_inactive;
    colors_inactive << QColor(0, 0, 0, 96) << QColor(0, 0, 0, 96);


    // map consists of index for vertical rect and color

    for(int i=0; i<n_rects; i++){
        insertColorOfRect(i, n_rects, borders_2, colors_inactive, _map_col_inactive);
    }

    foreach(QList<QColor> lst, colors_active){
        QMap<int, QColor> map;

        for(int i=0; i<n_rects; i++){
            if(lst.size() == 4)
                insertColorOfRect(i, n_rects, borders_4, lst, map);
            else if(lst.size() == 3)
                insertColorOfRect(i, n_rects, borders_3, lst, map);
            else if(lst.size() == 2)
                insertColorOfRect(i, n_rects, borders_2, lst, map);
        }

        _maps_col_active << map;
    }


    // scheme_fading_rect_color:
    // scheme_fading_rect_color[i]: get access to the fading scheme for theme i
    // scheme_fading_rect_color[i][r]: get access to the rect j in fading scheme i
    // scheme_fading_rect_color[i][r][c]: get access to the c-th color of rect j in fading scheme i

    // run through different styles
    QMap<int, QColor> map;

    foreach(map, _maps_col_active){

        QList< QMap<int, QColor> > rect_maps;

        QList<float> borders;

        borders << 0.0 << 1.0;


        for(int idx_rect=0; idx_rect < n_rects; idx_rect++){


            QColor active_color = map.value(idx_rect);
            QColor inactive_color = _map_col_inactive.value(idx_rect);

            // fadeout
            QMap<int, QColor> fading_map;
            QList<QColor> colors;
            //colors << QColor(255, 255, 255, 255) << QColor(0, 0, 0, 255);
            colors << inactive_color << active_color.darker();

            for(int idx_step=0; idx_step<=n_fading_steps; idx_step++)
                insertColorOfRect(idx_step, n_fading_steps, borders, colors, fading_map);

            rect_maps << fading_map;
        }

        _scheme_fading_rect_color << rect_maps;
    }


    for(int i=0; i<N_BINS; i++){

        _spec << 0.0f;
    }

    for(int i=0; i<1100; i++){
        log_lu[i] = log( (i * 1.0f) / 10.0f );
    }

    _steps = new int*[N_BINS];
    for(int i=0; i<N_BINS; i++){
        _steps[i] = new int[n_rects];
        for(int j=0; j<n_rects; j++){
            _steps[i][j] = 0;
        }
    }

    this->update();

}


void
GUI_Spectrum::mousePressEvent(QMouseEvent *e){

    if(e->button() == Qt::LeftButton){
        _cur_col = (_cur_col +  1) % _maps_col_active.size();
    }
    else if (e->button() == Qt::RightButton)
        _cur_col = (_cur_col > 0) ? (_cur_col - 1) : (_maps_col_active.size() - 1);
    else if (e->button() == Qt::MidButton){
        close();
        return;
    }

    CSettingsStorage::getInstance()->setSpectrumStyle(_cur_col);

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
    int offset = 0;
    if(ninety == 0) return;

    int w_bin = ((this->width()) / (ninety - offset)) - border_x;
    float widget_height = (float) this->height();

    for(int i=offset; i<ninety + 1; i++){


        float f = _spec[i] * log_lu[ i*10 + 54] * 0.60f;

        // if this is one bar, how tall would it be?
        int h =  f * widget_height;

        // how many colored rectangles would fit into this bar?
        int colored_rects = h / (h_rect + border_y)  -1 ;
        if (colored_rects < 0) colored_rects = 0;

        // we start from bottom with painting
        int y = widget_height - h_rect;


        for(int r=0; r<n_rects; r++){

            QColor col;

            if( r < colored_rects){
                col = _maps_col_active[_cur_col].value(r);
                _steps[i][r] = n_fading_steps;
            }

            else{
                col = _scheme_fading_rect_color[_cur_col][r].value(_steps[i][r]);
                if(!col.isValid()){
                    qDebug() << "Not valid on " << _cur_col << ", " << r << ", " << _steps[i][r];
                }

                if(_steps[i][r] > 0) _steps[i][r]--;

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
    this->update();
    emit sig_show(true);
}

void GUI_Spectrum::closeEvent(QCloseEvent *e){
    PlayerPlugin::closeEvent(e);
    this->update();
    emit sig_show(false);
}
