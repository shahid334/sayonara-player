/*
 * LibraryItemModel.h
 *
 *  Created on: 19.12.2012
 *      Author: lugmair
 */

#ifndef LIBRARYITEMMODEL_H_
#define LIBRARYITEMMODEL_H_



#define COL_ALBUM_MACROS
#define COL_ALBUM_SAMPLER 0
#define COL_ALBUM_NAME 1
#define COL_ALBUM_DURATION 2
#define COL_ALBUM_N_SONGS 3
#define COL_ALBUM_YEAR 4

#define COL_ARTIST_MACROS
#define COL_ARTIST_N_ALBUMS 0
#define COL_ARTIST_NAME 1
#define COL_ARTIST_TRACKS 2

#define COL_TRACK_NUM 0
#define COL_TITLE 1
#define COL_ARTIST 2
#define COL_ALBUM 3
#define COL_YEAR 4
#define COL_LENGTH 5
#define COL_BITRATE 6
#define COL_FILESIZE 7


#include <QAbstractTableModel>
#include <QModelIndex>
#include <QList>
#include <QString>
#include <QStringList>
#include <QVariant>

#include "GUI/MyColumnHeader.h"



class LibraryItemModel : public QAbstractTableModel{

	Q_OBJECT

public:

	LibraryItemModel(QList<ColumnHeader>& headers);
	virtual ~LibraryItemModel();


    QStringList 	get_header_names() const;
    int 			get_n_cols() const;
    int 			calc_shown_col(int col) const;
    bool			is_col_shown(int col) const;
    bool 			is_selected(int row) const;
    void 			set_selected(QList<int>& rows);


	/** Overloaded from QAbstractTableModel **/
    virtual QVariant 	headerData ( int section, Qt::Orientation orientation, int role ) const;
	virtual int 		columnCount(const QModelIndex& parent=QModelIndex()) const;
	virtual bool 		insertColumns(int position, int cols, const QModelIndex &index=QModelIndex());
	virtual bool 		removeColumns(int position, int cols, const QModelIndex &index=QModelIndex());

	virtual int			getFirstRowOf(QString substr)=0;




private:
    QStringList				_header_names;
    int						_n_all_cols;
    bool*                   _cols_active;

protected:
    QList<int>				_selected_rows;
};

#endif /* LIBRARYITEMMODEL_H_ */
