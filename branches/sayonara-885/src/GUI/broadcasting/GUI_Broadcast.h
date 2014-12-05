#ifndef GUI_BROADCAST_H
#define GUI_BROADCAST_H

#include "PlayerPlugin/PlayerPlugin.h"
#include "GUI/ui_GUI_Broadcast.h"


class GUI_Broadcast : public PlayerPlugin, private Ui::GUI_Broadcast
{
	Q_OBJECT

public:
	explicit GUI_Broadcast(QString name, QWidget *parent = 0);
	~GUI_Broadcast();

signals:
	void sig_dismiss(int);
	void sig_dismiss_all();
	void sig_accepted(const QString& ip);
	void sig_rejected(const QString& ip);

public slots:
	void new_connection_request(const QString& ip);
	void new_connection(const QString& ip);
	void connection_closed(const QString& ip);

private slots:
	void dismiss_clicked();
	void dismiss_all_clicked();
	void option_clicked();
	void combo_changed(int idx);

private:

	void dismiss_at(int idx);
	void set_status_label();



};

#endif // GUI_BROADCAST_H
