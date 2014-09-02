/* GUI_LevelPainter.h */

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



#ifndef GUI_LEVELPAINTER_H
#define GUI_LEVELPAINTER_H

#include "GUI/engine/EnginePlugin.h"
#include "GUI/ui_GUI_LevelPainter.h"
#include "GUI/engine/EngineColorStyleChooser.h"

#include <QString>

class GUI_LevelPainter : public EnginePlugin, private Ui::GUI_LevelPainter
{
    Q_OBJECT
public:
	explicit GUI_LevelPainter(QString name, QWidget *parent=0);

protected:
   void paintEvent(QPaintEvent* e);

protected slots:
	virtual void timed_out();

public slots:
    void set_level(float, float);
    virtual void psl_style_update();

private:

    float _level[2];

    int** _steps;

    void resize_steps(int n_rects);
    void reload();
};

#endif // GUI_LEVELPAINTER_H
