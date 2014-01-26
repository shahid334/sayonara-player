#ifndef MINISEARCHER_H
#define MINISEARCHER_H

#include <QFrame>
#include <QLineEdit>
#include <QBoxLayout>
#include <QPushButton>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QScrollArea>
#include "GUI/SearchableWidget.h"
#include <QMap>

enum MiniSearcherButtons{
  MiniSearcherNoButton=0,
  MiniSearcherFwdButton,
  MiniSearcherBwdButton,
  MiniSearcherBothButtons
};

class SearchableTableView;
class SearchableListView;
class MiniSearcher : public QFrame
{
    Q_OBJECT

signals:
    void sig_reset();
	void sig_text_changed(QString);
	void sig_find_next_row();
	void sig_find_prev_row();

private slots:

	void line_edit_text_changed(QString);
	void left_clicked();
	void right_clicked();


private:
    QAbstractScrollArea*    _parent;
	QMap<QChar, QString>    _triggers;
	QPushButton*            _left_button;
	QPushButton*            _right_button;
	QLineEdit*              _line_edit;
	QBoxLayout*             _layout;

    bool isInitiator(QKeyEvent* event);
    void init(QString text);
	void initLayout(MiniSearcherButtons b);

protected:
    void focusOutEvent(QFocusEvent *);

public:
	MiniSearcher(SearchableListView* parent, MiniSearcherButtons b=MiniSearcherNoButton);
	MiniSearcher(SearchableTableView *parent, MiniSearcherButtons b=MiniSearcherNoButton);


    bool isInitialized();
    bool check_and_init(QKeyEvent* event);
    void setExtraTriggers(QMap<QChar, QString> triggers);
	QString getCurrentText();
	void keyPressEvent(QKeyEvent *e);

public slots:
	void reset();


};

#endif // MINISEARCHER_H
