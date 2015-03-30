/* KeyConfigLineEdit.h */

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



#ifndef KEYCONFIGLINEEDIT_H
#define KEYCONFIGLINEEDIT_H

#include <QLineEdit>
#include <QKeyEvent>
#include <QKeySequence>

class KeyConfigLineEdit : public QLineEdit
{
	Q_OBJECT

signals:
	void sig_sequence_changed(const QKeySequence& sequence, int idx);
	void sig_sequence_cleared(int idx);

public:
	explicit KeyConfigLineEdit(QWidget *parent = 0);


protected:
	virtual void keyPressEvent(QKeyEvent* event);

	int _idx;
};

#endif // KEYCONFIGLINEEDIT_H
