#include "GUI/playlist/PlaylistTabWidget.h"

PlaylistTabMenu::PlaylistTabMenu(QWidget* parent) :
	QMenu(parent)
{
	_action_delete = new QAction(Helper::getIcon("delete"), tr("Delete"), this);
	_action_save = new QAction(Helper::getIcon("save"), tr("Save"), this);
	_action_save_as = new QAction(Helper::getIcon("save_as"), tr("Save as"), this);
	_action_close = new QAction(Helper::getIcon("power_off"), tr("Close"), this);
	_action_close_others = new QAction(Helper::getIcon("power_on"), tr("Close others"), this);

	QList<QAction*> actions;
	actions << _action_save << _action_save_as << _action_delete << _action_close_others << _action_close;

	this->addActions(actions);

	connect(_action_delete, SIGNAL(triggered()), SIGNAL(sig_delete_clicked()));
	connect(_action_save, SIGNAL(triggered()), SIGNAL(sig_save_clicked()));
	connect(_action_save_as, SIGNAL(triggered()), SIGNAL(sig_save_as_clicked()));
	connect(_action_close, SIGNAL(triggered()), SIGNAL(sig_close_clicked()));
	connect(_action_close_others, SIGNAL(triggered()), SIGNAL(sig_close_others_clicked()));
}

PlaylistTabMenu::~PlaylistTabMenu(){
	this->clear();
}

void PlaylistTabMenu::show_menu_items(bool save_action, bool save_as_action, bool delete_action){
	_action_delete->setVisible(delete_action);
	_action_save_as->setVisible(save_as_action);
	_action_save->setVisible(save_action);
}


void PlaylistTabMenu::show_close(bool b){
	_action_close->setVisible(b);
	_action_close_others->setVisible(b);
}





void PlaylistTabBar::save_pressed(){
	emit sig_tab_save(currentIndex());
}

void PlaylistTabBar::save_as_pressed(){
	int cur_idx = currentIndex();
	QString cur_text = tabText(cur_idx);

	QString name = QInputDialog::getText(
				this,
				tr("Save as..."),
				cur_text + ": " + tr("Save as"));

	emit sig_tab_save_as(currentIndex(), name);
}

void PlaylistTabBar::delete_pressed(){
	emit sig_tab_delete(currentIndex());
}

void PlaylistTabBar::close_pressed(){
	emit tabCloseRequested(this->currentIndex());
}

void PlaylistTabBar::close_others_pressed(){
	int my_tab = currentIndex();
	int i=0;
	while( count() > 1){
		if(i < my_tab){
			tabCloseRequested(0);
		}

		else if(i == my_tab){

		}

		else{
			tabCloseRequested(1);
		}

		i++;
	}
}

void PlaylistTabBar::mousePressEvent(QMouseEvent* e){

	if(e->button() == Qt::RightButton){

		int tab_nr = this->tabAt(e->pos());
		this->setCurrentIndex(tab_nr);
		emit sig_cur_idx_changed(tab_nr);

		_menu->exec(e->globalPos());
	}

	else {
		QTabBar::mousePressEvent(e);
	}
}



PlaylistTabBar::PlaylistTabBar(QWidget *parent) :
	QTabBar(parent)
{
	_menu = new PlaylistTabMenu(this);
	this->setDrawBase(false);

	connect(_menu, SIGNAL(sig_save_clicked()), this, SLOT(save_pressed()));
	connect(_menu, SIGNAL(sig_save_as_clicked()), this, SLOT(save_as_pressed()));
	connect(_menu, SIGNAL(sig_delete_clicked()), this, SLOT(delete_pressed()));
	connect(_menu, SIGNAL(sig_close_clicked()), this, SLOT(close_pressed()));
	connect(_menu, SIGNAL(sig_close_others_clicked()), this, SLOT(close_others_pressed()));
}


PlaylistTabBar::~PlaylistTabBar(){
	delete _menu;
}


void PlaylistTabBar::show_menu_items(bool save_action, bool save_as_action, bool delete_action){
	_menu->show_menu_items(save_action, save_as_action, delete_action);
}

void PlaylistTabBar::setTabsClosable(bool b){
	QTabBar::setTabsClosable(b);
	_menu->show_close(b);
}


PlaylistTabWidget::PlaylistTabWidget(QWidget* parent) :
	QTabWidget(parent)
{
	_tab_bar = new PlaylistTabBar(this);
	this->setTabBar(_tab_bar);

	connect(_tab_bar, SIGNAL(sig_tab_save(int)), this, SIGNAL(sig_tab_save(int)));
	connect(_tab_bar, SIGNAL(sig_tab_save_as(int, const QString&)), this, SIGNAL(sig_tab_save_as(int, const QString&)));
	connect(_tab_bar, SIGNAL(sig_tab_delete(int)), this, SIGNAL(sig_tab_delete(int)));
	connect(_tab_bar, SIGNAL(sig_cur_idx_changed(int)), this, SIGNAL(currentChanged(int)));
}

PlaylistTabWidget::~PlaylistTabWidget(){
	delete _tab_bar;
}

void PlaylistTabWidget::show_tabbar(){
	this->tabBar()->show();
}

void PlaylistTabWidget::hide_tabbar(){
	this->tabBar()->hide();
}

void PlaylistTabWidget::show_menu_items(bool save_action, bool save_as_action, bool delete_action){
	_tab_bar->show_menu_items(save_action, save_as_action, delete_action);
}

void PlaylistTabWidget::rename_tab(int idx, const QString& name){
	_tab_bar->setTabText(idx, name);
}

void PlaylistTabWidget::setTabsClosable(bool b){
	QTabWidget::setTabsClosable(b);
	_tab_bar->setTabsClosable(b);
}
