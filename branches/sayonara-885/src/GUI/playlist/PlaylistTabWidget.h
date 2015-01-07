#ifndef PLAYLISTTABBAR_H
#define PLAYLISTTABBAR_H

#include <QTabWidget>
#include <QTabBar>

class PlaylistTabWidget : public QTabWidget {

	Q_OBJECT

public:
	PlaylistTabWidget(QWidget* parent) : QTabWidget(parent){}

public:
	void show_tabbar(){ this->tabBar()->show(); }
	void hide_tabbar(){ this->tabBar()->hide(); }
	QTabBar* get_tabbar() {return this->tabBar();}
};

#endif // PLAYLISTTABBAR_H
