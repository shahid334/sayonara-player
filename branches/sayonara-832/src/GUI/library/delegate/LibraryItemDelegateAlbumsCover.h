#ifndef LIBRARYITEMDELEGATEALBUMSCOVER_H
#define LIBRARYITEMDELEGATEALBUMSCOVER_H

#include <QStyledItemDelegate>
#include "GUI/library/model/LibraryItemModelAlbums.h"
#include "GUI/library/view/LibraryView.h"

class LibraryItemDelegateAlbumsCover : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit LibraryItemDelegateAlbumsCover(LibraryItemModel* model, LibraryView* parent=0);
	virtual ~LibraryItemDelegateAlbumsCover();

	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

	void set_skin(bool dark);

private:

	QPen _pen;
	LibraryItemModel* _model;
	QColor _selected_background;
	
};

#endif // LIBRARYITEMDELEGATEALBUMSCOVER_H
