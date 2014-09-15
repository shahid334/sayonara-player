/* PlaylistLoader.h */

/* Copyright (C) 2011-2014  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



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
