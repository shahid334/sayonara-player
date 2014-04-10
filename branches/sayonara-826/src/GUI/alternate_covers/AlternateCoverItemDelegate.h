/* AlternateCoverItemDelegate.h */

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
 * AlternateCoverItemDelegate.h
 *
 *  Created on: Jul 1, 2011
 *      Author: luke
 */

#ifndef ALTERNATECOVERITEMDELEGATE_H_
#define ALTERNATECOVERITEMDELEGATE_H_

#include <QObject>
#include <QWidget>
#include <QTableView>
#include <QItemDelegate>
#include <QLabel>

class AlternateCoverItemDelegate : public QItemDelegate{

	Q_OBJECT
public:
    AlternateCoverItemDelegate(QObject* parent=0);
	virtual ~AlternateCoverItemDelegate();

public:

	    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;


		QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

		void setEditorData(QWidget *editor, const QModelIndex &index) const;
		void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private:

		int _m_column_width;
        QLabel* label;

};

#endif /* ALTERNATECOVERITEMDELEGATE_H_ */
