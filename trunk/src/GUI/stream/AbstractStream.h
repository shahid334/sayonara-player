#ifndef ABSTRACTSTREAM_H
#define ABSTRACTSTREAM_H

#include "PlayerPlugin/PlayerPlugin.h"
#include "Playlist/PlaylistHandler.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>

class AbstractStream : public PlayerPlugin
{

	Q_OBJECT

private:
	QLineEdit*				_le_url;
	QComboBox*				_combo_stream;
	QPushButton*			_btn_listen;
	QPushButton*			_btn_save;
	QPushButton*			_btn_delete;

	virtual void			init_connections();


signals:
	void sig_close_event();

public:
	AbstractStream(QString name, QWidget* parent=0);
	virtual ~AbstractStream();


protected:
	CDatabaseConnector*		_db;
	PlaylistHandler*		_playlist;

	QString					_title_fallback_name;

	int						_cur_station_idx;
	QString					_cur_station_name;
	QString					_cur_station_adress;

	// url, name
	QMap<QString, QString>	_stations;


	virtual void						language_changed();
	virtual void						play(QString url, QString station_name);
	virtual int							show_delete_confirm_dialog();

	virtual bool						get_all_streams(QMap<QString, QString>& result)=0;
	virtual bool						add_stream(QString name, QString url)=0;
	virtual bool						delete_stream(QString station_name)=0;

protected slots:
	virtual void listen_clicked();
	virtual void combo_idx_changed(int idx);
	virtual void combo_text_changed(const QString& text);
	virtual void url_text_changed(const QString& url);
	virtual void delete_clicked();
	virtual void save_clicked();


public:
	virtual void setup_stations(const QMap<QString, QString>&);

	template<typename T>
	void setup_parent(T* subclass){

		subclass->setupUi(subclass);
		_le_url =			subclass->le_url;
		_combo_stream =		subclass->combo_stream;
		_btn_listen =		subclass->btn_listen;
		_btn_save =			subclass->btn_save;
		_btn_delete =		subclass->btn_delete;

		init_connections();
	}
};

#endif // ABSTRACTSTREAM_H
