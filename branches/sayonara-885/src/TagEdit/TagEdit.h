#ifndef TAGEDIT_H
#define TAGEDIT_H

#include "HelperStructs/SayonaraClass.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QVector>

class TagEdit : public QObject, private SayonaraClass
{
	Q_OBJECT

signals:
	void sig_progress(int);

	void sig_metadata_received(const MetaDataList&);
	void sig_metadata_changed(const MetaDataList&);
	void sig_metadata_changed(int);


public:
	explicit TagEdit(QObject *parent = 0);

	void undo(int idx);
	void undo_all();

	MetaData get_metadata(int idx);
	MetaDataList get_all_metadata();

	int get_n_tracks();

	void update_track(int idx, const MetaData& md);
	void write_tracks_to_db();

public slots:
	void set_metadata(const MetaDataList& v_md);


private:
	MetaDataList _v_md;
	MetaDataList _v_md_orig;
	QVector<bool> _changed_md;

	CDatabaseConnector* _db;

	void check_for_new_artists_and_albums(QStringList& new_artists, QStringList& new_albums);

	void insert_new_artists(const QStringList& artists);
	void insert_new_albums(const QStringList& albums);

	void apply_artists_and_albums_to_md();


};

#endif // TAGEDIT_H
