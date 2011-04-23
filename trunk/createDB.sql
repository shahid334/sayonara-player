-- Current usage:
-- Start sqlite3 and paste statements
-- inspired by amarok
-- http://amarok.kde.org/wiki/Amarok_1.4/User_Guides/MySQL_HowTo

--table for artists
CREATE TABLE artists (
  artistID INTEGER PRIMARY KEY,  
  name VARCHAR(255)
);

--table for albums
CREATE TABLE albums (
  albumID INTEGER PRIMARY KEY,
  name VARCHAR(255)
);

--this table stores all imported directories, important for refresh issues
CREATE TABLE directories (
  path VARCHAR(1024) PRIMARY KEY,
  changedate INTEGER
);

--  TODO: not sure if needed. Would say if we can read out genres automatically
-- (tags, music brainz ...) we shall keep it
CREATE TABLE genres (
  genreID INTEGER PRIMARY KEY ,
  name VARCHAR(255)
);

-- Table storing lyrics. One entity might fit for more than one track
CREATE TABLE lyrics (
  lyricID INTEGER PRIMARY KEY,
  lyrics TEXT
);

-- Tracks 
CREATE TABLE Tracks (
  TrackID INTEGER PRIMARY KEY,
  filename VARCHAR(1024) unique, 	--could be pk as well
  path VARCHAR(1024),			--FK to dirs
  createdate INTEGER,			--read out of fs
  modifydate INTEGER,			--read out of fs
  filesize INTEGER,  			--out of fs  
  filetype INTEGER,  			--out of fs  
  albumID INTEGER,                      --FK to albums
  artistID INTEGER,			--FK to artists
  composer VARCHAR(255),		--out of id3
  title VARCHAR(255),                   --out of id3  
  year INTEGER,  			--out of id3  
  comment TEXT,  			--out of id3  
  track NUMERIC(4),  			--out of id3  
  discnumber INTEGER,  			--out of id3  
  bitrate INTEGER,  			--out of id3  
  length INTEGER,  			--out of id3  
  samplerate INTEGER,  			--out of id3  
  bpm FLOAT,	  			--out of id3  
  genreID INTEGER,			--FK to genres
  lyricID INTEGER,			--FK to lyrics
  foreign key (genreID) REFERENCES genres (genreID),
  foreign key (lyricID) REFERENCES lyrics (lyricID)
  foreign key (albumID) REFERENCES albums (albumID),
  foreign key (path) REFERENCES directories (path),
  foreign key (artistID) REFERENCES artists (artistID)
);

insert into Tracks (
  TrackID,
  filename, 	--could be pk as well
  path,			--FK to dirs
  createdate,			--read out of fs
  modifydate,			--read out of fs
  filesize,  			--out of fs  
  filetype,  			--out of fs  
  albumID,                      --FK to albums
  artistID,			--FK to artists
  composer,		--out of id3
  title,                   --out of id3  
  year,  			--out of id3  
  comment,  			--out of id3  
  track,  			--out of id3  
  discnumber,  			--out of id3  
  bitrate,  			--out of id3  
  length,  			--out of id3  
  samplerate,  			--out of id3  
  bpm,	  			--out of id3  
  genreID,			--FK to genres
  lyricID,			--FK to lyrics
) values (
  last_insert_rowid()+1,
  :filename,
  :path,
  :createdate,
  :modifydate,
  :filesize,
  :filetype,
  :albumID,
  :artistID,
  :composer,
  :title,
  :year,
  :comment,
  :track,
  :discnumber,
  :bitrate,
  :length,
  :samplerate,
  :bpm
)

	QString title;
	QString artist;
	QString album;
	qint32 rating;
        qint64 length_ms;
        qint32 year;
        QString filepath;












-- Stored playlists
CREATE TABLE playlists (
  playlistID INTEGER PRIMARY KEY,
  playlist VARCHAR(255)
);

-- mapping songs to playlists and vice versa
CREATE TABLE playlistToTracks (
  TrackID INTEGER,
  playlistID INTEGER,
  positionInPlaylist INTEGER unique,
  PRIMARY KEY (TrackID,playlistID),
  foreign key (TrackID) REFERENCES Tracks (TrackID),
  foreign key (playlistID) REFERENCES playlists (playlistID)
);

-- TODO statistic table... Should be used later
CREATE TABLE statistics (
  TrackID INTEGER PRIMARY KEY,
  createdate INTEGER,
  accessdate INTEGER,
  percentage FLOAT,
  rating INTEGER DEFAULT 0,
  playcounter INTEGER,
  foreign key (TrackID) REFERENCES Tracks (Tracks)
);