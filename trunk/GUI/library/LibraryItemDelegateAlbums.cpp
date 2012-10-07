/* LibraryItemDelegateAlbums.cpp */

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
 * LibraryItemDelegateAlbums.cpp
 *
 *  Created on: Apr 28, 2011
 *      Author: luke
 */

#include "GUI/library/LibraryItemDelegateAlbums.h"

#include <HelperStructs/MetaData.h>
#include <QObject>
#include <QLabel>
#include <QDebug>
#include <QTableView>
#include <QItemDelegate>
#include <QPainter>
#include "HelperStructs/Helper.h"


LibraryItemDelegateAlbums::LibraryItemDelegateAlbums(QTableView* parent) {
	this->_parent = parent;

        QString icon_path_no_sampler = Helper::getIconPath() + "play_small.png";
        QString icon_path_sampler = Helper::getIconPath() + "fwd_orange.png";

        QPalette palette = _parent->palette();
        QColor col_background = palette.color(QPalette::Active, QPalette::Background);
        QColor col_highlight = palette.color(QPalette::Active, QPalette::Highlight);
        _col_highlight_name = col_highlight.name();

        _val_bg = col_background.lightness();
        _val_sel = col_highlight.lightness();

        _icon_label_sampler = new QLabel();
        _icon_label_sampler->setPixmap(QPixmap(icon_path_sampler));
        _icon_label_sampler->setContentsMargins(2, 0, 2, 0);
        _icon_label_sampler->setAlignment( Qt::AlignVCenter );


        _icon_label_no_sampler = new QLabel();
        _icon_label_no_sampler->setPixmap(QPixmap(icon_path_no_sampler));
        _icon_label_no_sampler->setContentsMargins(2, 0, 2, 0);
        _icon_label_no_sampler->setAlignment( Qt::AlignVCenter );

        _label = new QLabel();
        _label->setContentsMargins(2, 0, 2, 0);
}

LibraryItemDelegateAlbums::~LibraryItemDelegateAlbums() {
    delete _label;
    delete _icon_label_sampler;
    delete _icon_label_no_sampler;
}


void LibraryItemDelegateAlbums::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{

	if(!index.isValid()) return;

	QRect rect(option.rect);

        painter->save();
	painter->translate(0, 0);


        QLabel* label = _label;
        QString label_text;
        int row_height = _parent->rowHeight(index.row());
        int col_width = _parent->columnWidth(index.column());

        int column = index.column();


        if(column == 0){

		bool is_sampler = index.model()->data(index, Qt::WhatsThisRole).toBool();


                if(is_sampler)
                    label = _icon_label_sampler;

                else
                    label = _icon_label_no_sampler;


                label->setFixedHeight(row_height);
                label->setFixedWidth(col_width);
                label->render(painter, rect.topLeft() );
                painter->restore();
                return;
	}


        else if(column == 1){

		QStringList list = index.model()->data(index, Qt::WhatsThisRole).toStringList();
		Album album;
		album.fromStringList(list);

                label_text = QString("<b>") + album.name + "</b>";// - " + QString::number(album.num_songs) + " tracks, " + Helper::cvtMsecs2TitleLengthString(album.length_sec * 1000);
                label->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );

	}

	else{
                label_text  = index.model()->data(index, Qt::WhatsThisRole).toString();
                if(index.model()->data(index, Qt::WhatsThisRole).toInt() == 0) label_text = "Unknown";


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


QSize LibraryItemDelegateAlbums::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	Q_UNUSED(option);
	Q_UNUSED(index);

	return QSize(1, _parent->rowHeight(index.row()));
}


void LibraryItemDelegateAlbums::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex & index) const
{
	Q_UNUSED(editor);
	Q_UNUSED(model);
	Q_UNUSED(index);
}

QWidget *LibraryItemDelegateAlbums::createEditor(QWidget *parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	Q_UNUSED(parent);
	Q_UNUSED(option);
	Q_UNUSED(index);
	return 0;
}


void LibraryItemDelegateAlbums::setEditorData(QWidget *editor, const QModelIndex & index) const
{
	Q_UNUSED(editor);
	Q_UNUSED(index);
}


