#ifndef DISC_POPUP_MENU_H_
#define DISC_POPUP_MENU_H_

#include <QMenu>
#include <QAction>
#include <QList>
#include <QString>
#include <QDebug>
#include <QMouseEvent>


class DiscAction : public QAction {

Q_OBJECT

	signals:
		void sig_disc_pressed(int);

	private slots:
		void disc_hover(){
			bool ok = false;
			int discnumber = data().toInt(&ok);
			if(ok)	
				emit sig_disc_pressed(discnumber);

			
		}

	public:
		DiscAction(QWidget* parent) : QAction(parent){ 
			connect(this, SIGNAL(triggered()), this, SLOT(disc_hover()));
		}
};


class DiscPopupMenu : public QMenu {

	Q_OBJECT

	signals:
		void sig_disc_pressed(int);

	private slots:
		void disc_pressed(int disc){
			emit sig_disc_pressed(disc);
		}


	public: 
		DiscPopupMenu(QWidget* parent, QList<int> discs): QMenu(parent){

			for(int i= -1; i<discs.size(); i++){
				QString text;
				int data;

				if(i == -1) {
					text = "All";
					data = -1;
				}

				else{
					int disc = discs[i];
					text = QString("Disc ") + QString::number(disc);
					data = disc;
				}

				DiscAction* action = new DiscAction(this);
				action->setText(text);
				action->setData(data);
				
				connect(action, SIGNAL(sig_disc_pressed(int)), this, SLOT(disc_pressed(int)));
				addAction(action);
				_actions.push_back(action);
			}	
		}

		~DiscPopupMenu(){
			clean_up();
		}

	protected: 
		void mouseReleaseEvent(QMouseEvent* e){
			QMenu::mouseReleaseEvent(e);
			hide();
			close();
		}

	private:
		QList<DiscAction*> _actions;

		void clean_up(){
			foreach(DiscAction* a, _actions){
				if(!a) continue;
				delete a;
				a = 0;
			}
		}
};





#endif
