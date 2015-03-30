/* GUI_KeyConfig.h */

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



#ifndef GUI_KEYCONFIG_H
#define GUI_KEYCONFIG_H
#include <QDebug>

#include "HelperStructs/SayonaraClass.h"
#include "GUI/ui_GUI_KeyConfig.h"
#include <QKeyEvent>
#include <QtDBus/QtDBus>

class GUI_KeyConfig : public SayonaraWidget, protected Ui::GUI_KeyConfig
{
	Q_OBJECT

public:
	explicit GUI_KeyConfig(QWidget *parent = 0);


};

#endif // GUI_KEYCONFIG_H
