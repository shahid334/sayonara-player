/* LibraryItemDelegateArtists.cpp */

/* Copyright (C) 2011  Lucio Carreras
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


/*
 * LibraryItemDelegateArtists.cpp
 *
 *  Created on: Apr 28, 2011
 *      Author: luke
 */

#include "GUI/library/LibraryItemDelegateArtists.h"
#include <HelperStructs/MetaData.h>
#include <QObject>
#include <QLabel>
#include <QDebug>
#include <QTableView>
#include <QItemDelegate>
#include <QPainter>
#include <HelperStructs/Helper.h>

LibraryItemDelegateArtists::LibraryItemDelegateArtists(QTableView* parent) {
	this->_parent = parent;


        QPixmap icon_single_album(Helper::getIconPath() + "play_small.png");
        QPixmap icon_multi_album(Helper::getIconPath() + "fwd_orange.png");


        QPalette palette = _parent->palette();
        QColor col_background = palette.color(QPalette::Active, QPalette::Background);
        QColor col_highlight = palette.color(QPalette::Active, QPalette::Highlight);
        //QColor col_highlight_lighter = palette.color(QPalette::Active, QPalette::Highlight).light();
        _col_highlight_name = col_highlight.name();
        _val_bg = col_background.lightness();
        _val_sel = col_highlight.lightness();

        _icon_label_single_album = new QLabel();
        _icon_label_single_album->setAlignment( Qt::AlignVCenter );
        _icon_label_single_album->setContentsMargins(2, 0, 2, 0);
        _icon_label_single_album->setPixmap(icon_single_album);

        _icon_label_multi_album = new QLabel();
        _icon_label_multi_album->setContentsMargins(2, 0, 2, 0);
        _icon_label_multi_album->setAlignment( Qt::AlignVCenter );
        _icon_label_multi_album->setPixmap(icon_multi_album);

        _label = new QLabel();

        _label->setContentsMargins(2, 0, 2, 0);


}

LibraryItemDelegateArtists::~LibraryItemDelegateArtists() {
	// TODO Auto-generated destructor stub
}


void LibraryItemDelegateArtists::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{

	if(!index.isValid()) return;

                QRect rect(option.rect);
                QLabel* label = _label;
                QString label_text;

                int row_height = _parent->rowHeight(index.row())-1;
                int col_width = _parent->columnWidth(index.column());

		painter->save();
		painter->translate(0, 0);

		if(index.column() == 0){
                        int num_albums = index.model()->data(index, Qt::WhatsThisRole).toInt();

                        if(num_albums <= 1)
                           label = _icon_label_single_album;

                        else
                            label = _icon_label_multi_album;


                        label->setFixedHeight(row_height);
                        label->setFixedWidth(col_width);
                        label->render(painter, rect.topLeft() );

                        painter->restore();
                        return;
		}

		else if(index.column() == 1){

			QStringList list = index.model()->data(index, Qt::WhatsThisRole).toStringList();
			Artist artist;
			artist.fromStringList(list);

                        label_text = "<b>" + artist.name + "</b>";
                        label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		}

		else if(index.column() == 2){
			QString text = index.model()->data(index, Qt::WhatsThisRole).toString();
                        label_text = text + " tracks";
                        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		}

		QString style;
		QString fg_color;

		bool is_selected = ((option.state & QStyle::State_Selected) != 0);


		if(!is_selected){

                        if(_val_bg > 96) fg_color = " color: #202020";
			else fg_color = " color: #D8D8D8; ";

			style = QString("background-color: transparent; ") + fg_color;
		}

		else {
                        if(_val_sel > 96) fg_color = " color: #202020";
			else fg_color = " color: #D8D8D8; ";

                        style = QString("background-color: " + _col_highlight_name + ";") + fg_color;
		}

                label->setText(label_text);
                label->setStyleSheet(style);
                label->setFixedHeight(row_height);
                label->setFixedWidth(col_width);
                label->render(painter, rect.topLeft() );

		painter->restore();
}





QSize LibraryItemDelegateArtists::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{


	Q_UNUSED(option);
	Q_UNUSED(index);

        delete _label;
        delete _icon_label_single_album;
        delete _icon_label_multi_album;

	return QSize(1, _parent->rowHeight(index.row()));


}




void LibraryItemDelegateArtists::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex & index) const
{
	Q_UNUSED(editor);
	Q_UNUSED(model);
	Q_UNUSED(index);
}

QWidget *LibraryItemDelegateArtists::createEditor(QWidget *parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	Q_UNUSED(parent);
	Q_UNUSED(option);
	Q_UNUSED(index);
	return 0;
}




void LibraryItemDelegateArtists::setEditorData(QWidget *editor, const QModelIndex & index) const
{
	Q_UNUSED(editor);
	Q_UNUSED(index);
}


