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




