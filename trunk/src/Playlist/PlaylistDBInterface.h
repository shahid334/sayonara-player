#ifndef PLAYLISTDBINTERFACE_H
#define PLAYLISTDBINTERFACE_H

#include "Playlist/PlaylistDBConnector.h"
#include "HelperStructs/MetaData/MetaData.h"

class PlaylistDBInterface : public QObject {

private:

	PlaylistDBConnector*	_playlist_db_connector;
	QString					_name;
	bool					_is_temporary;
	int						_db_id;

public:

	enum SaveAsAnswer {
		SaveAs_Success=0,
		SaveAs_AlreadyThere,
		SaveAs_ExternTracksError,
		SaveAs_Error
	};

	PlaylistDBInterface(const QString& name);

	int get_db_id() const;
	void set_db_id(int db_id);

	QString get_name() const;
	void set_name(const QString& name);

	bool is_temporary() const;
	void set_temporary(bool b);

	void set_skeleton(const CustomPlaylistSkeleton& skeleton);

	bool insert_temporary_into_db();
	SaveAsAnswer save();
	SaveAsAnswer save_as(const QString& str, bool force_override=false);
	bool remove_from_db();


	virtual const MetaDataList& get_playlist() const = 0;

	static QString request_new_db_name();
};

#endif // PLAYLISTDBINTERFACE_H
