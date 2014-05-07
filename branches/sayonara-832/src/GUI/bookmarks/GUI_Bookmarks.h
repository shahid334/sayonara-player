#ifndef GUI_BOOKMARKS_H
#define GUI_BOOKMARKS_H


#include "DatabaseAccess/CDatabaseConnector.h"
#include "HelperStructs/MetaData.h"
#include "PlayerPlugin/PlayerPlugin.h"
#include "GUI/ui_GUI_Bookmarks.h"
#include <QMap>

class GUI_Bookmarks : public PlayerPlugin, private Ui::GUI_Bookmarks
{
	Q_OBJECT
public:
	explicit GUI_Bookmarks(QString name, QWidget *parent = 0);
	

signals:
	void sig_bookmark(quint32);
	
public slots:
	void track_changed(const MetaData&);
	void pos_changed_s(quint32);

private slots:
	void combo_changed(int new_idx);
	void next_clicked();
	void prev_clicked();
	void new_clicked();
	void del_clicked();
	void del_all_clicked();


private:
	Ui::GUI_Bookmarks* ui;
	CDatabaseConnector* _db;
	QMap<quint32, QString> _bookmarks;

	quint32 _cur_time;
	MetaData _md;

	void calc_prev(quint32 time_s);
	void calc_next(quint32 time_s);

	int _last_idx;
	int _next_idx;

	void enable_prev(int idx);
	void enable_next(int idx);
	void disable_prev();
	void disable_next();

	void set_idx(int idx);
	
};

#endif // GUI_BOOKMARKS_H
