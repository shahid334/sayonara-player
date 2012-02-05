/*
 * LyricMenuButton.h
 *
 *  Created on: Feb 5, 2012
 *      Author: luke
 */

#ifndef LYRICMENUBUTTON_H_
#define LYRICMENUBUTTON_H_

#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QList>
#include <QStringList>

class LyricMenuButton : public QToolButton {
	Q_OBJECT

public:
	LyricMenuButton(QWidget* parent);
	virtual ~LyricMenuButton();
	void setServers(const QStringList&);


signals:
	void sig_server_changed(int);


private slots:
	void server_changed(QAction*);


private:
	QList<QAction*>	_actions;
	QMenu*			_menu;
	QStringList		_servers;



};

#endif /* LYRICMENUBUTTON_H_ */
