#ifndef ENGINECOLORSTYLECHOOSER_H
#define ENGINECOLORSTYLECHOOSER_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QColor>
#include "GUI/engine/StyleTypes.h"


class EngineColorStyleChooser : public QObject
{
    Q_OBJECT

signals:
    void sig_use_style(int, const ColorStyle& );

public:
    EngineColorStyleChooser(int widget_width, int widget_height);
    ColorStyle get_color_scheme_spectrum(int i);
    ColorStyle get_color_scheme_level(int i);
    int get_num_color_schemes();
    void change_preset(const ColorList& lst, int n_rects, int n_fading_steps);
    void preview_preset(const ColorList& lst, int n_rects, int n_fading_steps);
    void delete_preset(QString name);
    void add_rectangles(ColorStyle& style, int n);
    void reload(int widget_width, int widget_height);



private:
    void insertColorOfRect(int bin, int n_bins, const ColorList& colors, QMap<int, QColor>& map);
    void create_colorstyle(ColorStyle &style, const ColorList &colors_active, int n_rects, int n_fading_steps);
    void init();


    QList< ColorStyle > _styles_spectrum;
    QList< ColorStyle > _styles_level;

};

#endif // ENGINECOLORSTYLECHOOSER_H
