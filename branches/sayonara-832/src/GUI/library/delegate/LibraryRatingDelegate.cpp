/* LibraryRatingDelegate.cpp */

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



#include "LibraryRatingDelegate.h"
#include "GUI/RatingLabel.h"
#include <QDebug>

LibraryRatingDelegate::LibraryRatingDelegate(LibraryItemModel* model, LibraryView* parent)

{
    _parent = parent;
}



QWidget *LibraryRatingDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{

    RatingLabel *label = new RatingLabel(parent);

    connect(label, SIGNAL(sig_finished(bool)), this, SLOT(destroy_editor(bool)));

    label->set_rating(index.data().toInt());

    _parent->set_editor(label);

    return label;
}


void LibraryRatingDelegate::destroy_editor(bool save){

	_parent->set_editor(NULL);
    RatingLabel *label = qobject_cast<RatingLabel *>(sender());
    if(!label) return;

    disconnect(label, SIGNAL(sig_finished(bool)), this, SLOT(destroy_editor(bool)));

	emit commitData(label);
	emit sig_rating_changed(label->get_rating().get_rating());
    emit closeEditor(label);
}


void LibraryRatingDelegate::setEditorData(QWidget *editor, const QModelIndex & index) const
{
	int rating = index.data().toInt();

	RatingLabel* label = qobject_cast<RatingLabel *>(editor);
	if(!label) return;

	label->set_rating(rating);
}


void LibraryRatingDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex & index) const
{

    RatingLabel* label = qobject_cast<RatingLabel *>(editor);
    if(!label) return;
    model->setData(index, label->get_rating().get_rating());
}




