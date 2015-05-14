#ifndef MENUTOOL_H
#define MENUTOOL_H

#include <QMenu>
#include <QPushButton>
#include <QMouseEvent>
#include <QList>
#include <QAction>
#include "HelperStructs/SayonaraClass.h"

class MenuToolMenu : public QMenu, protected SayonaraClass
{
	Q_OBJECT

signals:
	void sig_new();
	void sig_undo();
	void sig_save();
	void sig_save_as();
	void sig_rename();
	void sig_delete();
	void sig_open();


private:
	QAction*	_action_new;
	QAction*	_action_open;
	QAction*	_action_undo;
	QAction*	_action_save;
	QAction*	_action_save_as;
	QAction*	_action_rename;
	QAction*	_action_delete;


	QList<QAction*> _actions;

	void show_action(bool b, QAction* action);


public:
	explicit MenuToolMenu(QWidget *parent = 0);
	bool prove_valid();

public slots:
	void show_open(bool b);
	void show_new(bool b);
	void show_undo(bool b);
	void show_save(bool b);
	void show_save_as(bool b);
	void show_rename(bool b);
	void show_delete(bool b);
	void show_all(bool b);
};


class MenuToolButton : public QPushButton, protected SayonaraClass {

	Q_OBJECT

signals:
	void sig_open();
	void sig_new();
	void sig_undo();
	void sig_save();
	void sig_save_as();
	void sig_rename();
	void sig_delete();

public:
	explicit MenuToolButton(QWidget *parent = 0);
	explicit MenuToolButton(bool show_all=false, QWidget *parent = 0);

private:
	MenuToolMenu* _menu;
	void set_std_icon();


public slots:
	void show_open(bool b);
	void show_new(bool b);
	void show_undo(bool b);
	void show_save(bool b);
	void show_save_as(bool b);
	void show_rename(bool b);
	void show_delete(bool b);
	void show_all(bool b);


protected:

	void mouseReleaseEvent(QMouseEvent *e);
	void mousePressEvent(QMouseEvent* e);
	void enterEvent(QEvent* e);
	void leaveEvent(QEvent* e);
	bool prove_enabled();


protected slots:
	void _sl_skin_changed();


};


#endif // MENUTOOL_H
