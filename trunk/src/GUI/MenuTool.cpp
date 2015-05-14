#include "MenuTool.h"
#include "HelperStructs/Style/Style.h"
#include "HelperStructs/Helper.h"

MenuToolButton::MenuToolButton(QWidget *parent) :
	MenuToolButton::MenuToolButton(false, parent)
{
}

MenuToolButton::MenuToolButton(bool show_all, QWidget *parent) :
	QPushButton(parent),
	SayonaraClass()
{
	_menu = new MenuToolMenu(this);

	connect(_menu, SIGNAL(sig_open()), this, SIGNAL(sig_open()));
	connect(_menu, SIGNAL(sig_new()), this, SIGNAL(sig_new()));
	connect(_menu, SIGNAL(sig_undo()), this, SIGNAL(sig_undo()));
	connect(_menu, SIGNAL(sig_save()), this, SIGNAL(sig_save()));
	connect(_menu, SIGNAL(sig_save_as()), this, SIGNAL(sig_save_as()));
	connect(_menu, SIGNAL(sig_rename()), this, SIGNAL(sig_rename()));
	connect(_menu, SIGNAL(sig_delete()), this, SIGNAL(sig_delete()));


	this->setFlat(true);
	this->set_std_icon();

	_menu->show_all(show_all);

	prove_enabled();

	REGISTER_LISTENER(Set::Player_Style, _sl_skin_changed);
}

void MenuToolButton::set_std_icon(){

	bool dark = (_settings->get(Set::Player_Style) == 1);

	QIcon icon;
	QPixmap pixmap;
	QPixmap pixmap_disabled;

	if(dark){
		pixmap = Helper::getPixmap("tool_dark_grey");
		pixmap_disabled = Helper::getPixmap("tool_disabled");
	}

	else{
		pixmap = Helper::getPixmap("edit");
		pixmap_disabled = Helper::getPixmap("edit");
	}

	icon.addPixmap(pixmap, QIcon::Normal, QIcon::On);
	icon.addPixmap(pixmap, QIcon::Normal, QIcon::Off);
	icon.addPixmap(pixmap_disabled, QIcon::Disabled, QIcon::On);
	icon.addPixmap(pixmap_disabled, QIcon::Disabled, QIcon::Off);
	icon.addPixmap(pixmap, QIcon::Active, QIcon::On);
	icon.addPixmap(pixmap, QIcon::Active, QIcon::Off);
	icon.addPixmap(pixmap, QIcon::Selected, QIcon::On);
	icon.addPixmap(pixmap, QIcon::Selected, QIcon::Off);
	this->setIcon(icon);
}


void MenuToolButton::show_open(bool b){
	_menu->show_open(b);
	prove_enabled();
}

void MenuToolButton::show_new(bool b){
	_menu->show_new(b);
	prove_enabled();
}

void MenuToolButton::show_undo(bool b){
	_menu->show_undo(b);
	prove_enabled();
}

void MenuToolButton::show_save(bool b){
	_menu->show_save(b);
	prove_enabled();
}

void MenuToolButton::show_save_as(bool b){
	_menu->show_save_as(b);
	prove_enabled();
}

void MenuToolButton::show_rename(bool b){
	_menu->show_rename(b);
	prove_enabled();
}

void MenuToolButton::show_delete(bool b){
	_menu->show_delete(b);
	prove_enabled();
}

void MenuToolButton::show_all(bool b){
	_menu->show_all(b);
	prove_enabled();
}

bool MenuToolButton::prove_enabled(){
	bool enabled = _menu->prove_valid();
	this->setEnabled(enabled);
	return enabled;
}


void MenuToolButton::mousePressEvent(QMouseEvent* e){
	e->accept();

	QPoint globalPoint = this->mapToGlobal(this->pos()) - this->pos();
	_menu->popup(globalPoint);

}

void MenuToolButton::mouseReleaseEvent(QMouseEvent *e){
	e->accept();
}


void MenuToolButton::enterEvent(QEvent* e){

	bool dark = (_settings->get(Set::Player_Style) == 1);

	QIcon icon = Helper::getIcon("edit");
	if(dark){
		icon = Helper::getIcon("tool_grey");
	}

	if( this->isEnabled() ){
		this->setIcon(icon);
		e->accept();
	}

}

void MenuToolButton::leaveEvent(QEvent* e){
	e->accept();

	set_std_icon();
}



void MenuToolButton::_sl_skin_changed(){

	set_std_icon();
}




MenuToolMenu::MenuToolMenu(QWidget *parent) :
	QMenu(parent)
{
	_action_open = new QAction(Helper::getIcon("open"), tr("Open"), this);
	_action_new = new QAction(Helper::getIcon("new"), tr("New"), this);
	_action_undo = new QAction(Helper::getIcon("undo"), tr("Undo"), this);
	_action_save = new QAction(Helper::getIcon("save"), tr("Save"), this);
	_action_save_as = new QAction(Helper::getIcon("save_as"), tr("Save as"), this);
	_action_rename = new QAction(Helper::getIcon("edit"), tr("Rename"), this);
	_action_delete = new QAction(Helper::getIcon("delete"), tr("Delete"), this);


	_actions << addSeparator()
			<< _action_new
			<< _action_open
			<< _action_save
			<< _action_save_as
			<< _action_rename
			<< addSeparator()
			<< _action_undo
			<< addSeparator()
			<< _action_delete;

	this->addActions(_actions);


	connect(_action_open, SIGNAL(triggered()), this, SIGNAL(sig_open()));
	connect(_action_new, SIGNAL(triggered()), this, SIGNAL(sig_new()));
	connect(_action_undo, SIGNAL(triggered()), this, SIGNAL(sig_undo()));
	connect(_action_save, SIGNAL(triggered()), this, SIGNAL(sig_save()));
	connect(_action_save_as, SIGNAL(triggered()), this, SIGNAL(sig_save_as()));
	connect(_action_rename, SIGNAL(triggered()), this, SIGNAL(sig_rename()));
	connect(_action_delete, SIGNAL(triggered()), this, SIGNAL(sig_delete()));
}


void MenuToolMenu::show_open(bool b){
	_action_open->setVisible(b);
}

void MenuToolMenu::show_new(bool b){
	_action_new->setVisible(b);
}

void MenuToolMenu::show_undo(bool b){
	_action_undo->setVisible(b);
}

void MenuToolMenu::show_save(bool b){
	_action_save->setVisible(b);
}

void MenuToolMenu::show_save_as(bool b){
	_action_save_as->setVisible(b);
}

void MenuToolMenu::show_rename(bool b){
	_action_rename->setVisible(b);
}

void MenuToolMenu::show_delete(bool b){
	_action_delete->setVisible(b);
}

bool MenuToolMenu::prove_valid() {
	for(QAction* action: _actions){
		if(action->isVisible()){
			return true;
		}
	}

	return false;
}



void MenuToolMenu::show_all(bool b){
	for(QAction* action: _actions){
		action->setVisible(b);
	}
}

