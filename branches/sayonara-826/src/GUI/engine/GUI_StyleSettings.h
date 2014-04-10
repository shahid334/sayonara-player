/* GUI_StyleSettings.h */

/* Copyright (C) 2013  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



#ifndef STYLESETTINGS_H
#define STYLESETTINGS_H

#include <QWidget>
#include <QList>
#include <QCloseEvent>

#include "GUI/ui_GUI_Style.h"
#include "GUI/engine/StyleTypes.h"
#include "DatabaseAccess/CDatabaseConnector.h"

class GUI_StyleSettings : public QDialog, private Ui::GUI_Style
{
    Q_OBJECT
public:
    explicit GUI_StyleSettings(QWidget *parent = 0);
    
signals:
    void sig_style_update();
    
public slots:
    void show(int);
    void language_changed();

private slots:
    void combo_styles_changed(int);
    void combo_text_changed(const QString&);
    void col1_activated();
    void col2_activated();
    void col3_activated();
    void col4_activated();
    void save_pressed();
    void del_pressed();
    void undo_pressed();
    void spin_box_changed(int);
    void col_changed();


private:
    Ui::GUI_Style* ui;

    CDatabaseConnector* _db;
    QList<RawColorStyle> _styles;
    QList<RawColorStyle> _styles_old;
    RawColorStyle _cur_style;
    QColor _colors[4];
    bool _sth_changed;
    QString _cur_text;
    int _cur_idx;
    void init();

    void connect_spinboxes();
    void disconnect_spinboxes();
    void set_sth_changed(bool b);

protected:
    void closeEvent(QCloseEvent *);
    
};

#endif // STYLESETTINGS_H
