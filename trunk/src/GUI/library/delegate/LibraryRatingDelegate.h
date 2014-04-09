#ifndef LIBRARY_RATING_DELEGATE_H
#define LIBRARY_RATING_DELEGATE_H

#include <QStyledItemDelegate>
#include "GUI/library/view/LibraryView.h"
#include "GUI/library/model/LibraryItemModel.h"

class LibraryRatingDelegate : public QStyledItemDelegate
{
    Q_OBJECT

signals:
    void sig_rating_changed(int);

public:
    LibraryRatingDelegate(LibraryItemModel* model, LibraryView* parent=0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    

private slots:
    void destroy_editor(bool save=false);


protected:
    LibraryView* _parent;
    
};

#endif // LibraryRatingDelegate_H
