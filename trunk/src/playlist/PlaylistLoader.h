#ifndef PLAYLISTLOADER_H
#define PLAYLISTLOADER_H

#include <QObject>
#include "HelperStructs/CSettingsStorage.h"

class PlaylistLoader : public QObject
{
    Q_OBJECT
public:
    explicit PlaylistLoader(QObject *parent = 0);
    void load_old_playlist();
    
signals:
    void sig_create_playlist(MetaDataList&, bool);
    void sig_change_track(int, qint32, bool);
    void sig_stop();
    
public slots:

private:
    CSettingsStorage* _settings;
    
};

#endif // PLAYLISTLOADER_H
