#ifndef LIBRARYVIEWALBUM_H
#define LIBRARYVIEWALBUM_H

#include <QTimer>

#include "HelperStructs/Album.h"
#include "GUI/library/view/LibraryView.h"
#include "GUI/library/DiscPopupMenu.h"

class LibraryViewAlbum : public LibraryView
{
	Q_OBJECT

signals:
	void sig_disc_pressed(int);

protected slots:
	virtual void disc_pressed(int);
	virtual void timer_timed_out();

public:
	explicit LibraryViewAlbum(QWidget *parent = 0);

	template <class TList, class T>
	void fill(const AlbumList& input_data);

protected:
	QVector< QList<quint8> >	_discnumbers;
	DiscPopupMenu*				_discmenu;
	QPoint						_discmenu_point;
	QTimer*						_timer;


	virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected );

	virtual void calc_discmenu_point(QModelIndex idx);
	virtual void delete_discmenu();
	virtual void init_discmenu(QModelIndex idx);
	virtual void show_discmenu();

	virtual void rc_menu_show(const QPoint& p);
	virtual void mousePressEvent(QMouseEvent* e);

};

#endif // LIBRARYVIEWALBUM_H
