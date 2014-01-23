#ifndef MINISEARCHER_H
#define MINISEARCHER_H

#include <QLineEdit>
#include <QKeyEvent>
#include <QScrollArea>
#include <GUI/SearchableWidget.h>


class MiniSearcher : public QLineEdit
{
    Q_OBJECT

signals:
    void sig_reset();



private:
    SearchableTableView* _parent;
    bool _initialized;

    void init(QString text);

private slots:
    void key_pressed(QKeyEvent* event);

public slots:
    void reset();

public:
    explicit MiniSearcher(SearchableTableView *parent);


    bool is_initiator(QKeyEvent* event);

};

#endif // MINISEARCHER_H
