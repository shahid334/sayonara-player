#ifndef MINISEARCHER_H
#define MINISEARCHER_H

#include <QLineEdit>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QScrollArea>
#include "GUI/SearchableWidget.h"
#include <QList>


class SearchableTableView;
class SearchableListView;
class MiniSearcher : public QLineEdit
{
    Q_OBJECT

signals:
    void sig_reset();

private slots:
    void reset();


private:
    QAbstractScrollArea*    _parent;
    QList<QChar> _triggers;

    bool isInitiator(QKeyEvent* event);
    void init(QString text);

protected:
    void focusOutEvent(QFocusEvent *);
    void keyPressEvent(QKeyEvent *);

public:
    MiniSearcher(SearchableListView* parent);
    MiniSearcher(SearchableTableView *parent);


    bool isInitialized();
    bool check_and_init(QKeyEvent* event);
    void register_extra_keys(QList<QChar> keys);


};

#endif // MINISEARCHER_H
