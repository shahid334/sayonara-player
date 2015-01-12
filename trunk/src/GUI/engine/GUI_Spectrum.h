/* GUI_Spectrum.h */

/* Copyright (C) 2011-2014  Lucio Carreras
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



#ifndef GUI_SPECTRUM_H
#define GUI_SPECTRUM_H

#include "GUI/engine/EnginePlugin.h"
#include "GUI/engine/EngineColorStyleChooser.h"
#include "GUI/ui_GUI_Spectrum.h"

#include <QList>
#include <QString>

class GUI_Spectrum : public EnginePlugin, private Ui::GUI_Spectrum
{
    Q_OBJECT
public:
	explicit GUI_Spectrum(QString name, QWidget *parent=0);

protected:
    void paintEvent(QPaintEvent* e);
	void showEvent(QShowEvent*);
	void closeEvent(QCloseEvent*);

protected slots:
    virtual void timed_out();


public slots:
    void set_spectrum(QList<float>&);
    virtual void psl_style_update();

private:

    QList<float> _spec;
    int** _steps;

    void resize_steps(int bins, int rects);

};

#endif // GUI_SPECTRUM_H
