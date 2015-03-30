/* KeyConfigLineEdit.cpp */

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



#include "GUI/KeyConfig/KeyConfigLineEdit.h"
#include <QDebug>

KeyConfigLineEdit::KeyConfigLineEdit(QWidget *parent) :
	QLineEdit(parent)
{
	this->setPlaceholderText(tr("Please type shortcut"));
}


void KeyConfigLineEdit::keyPressEvent(QKeyEvent* e){

	if(e->key() == Qt::Key_Escape){
		this->setText("");
		this->setPlaceholderText(tr("Please type shortcut"));
		emit sig_sequence_cleared(_idx);
	}

	else{

		QKeySequence sequence(e->modifiers() + e->key());

		if( e->key() == Qt::Key_Control ||
			e->key() == Qt::Key_Alt  ||
			e->key() == Qt::Key_Meta ||
			e->key() == Qt::Key_Shift)
		{
			sequence = QKeySequence (e->modifiers());
		}





		qDebug() << e->key() << "   " << e->modifiers();

		this->setText(sequence.toString());
		emit sig_sequence_changed(sequence, _idx);
	}
}
