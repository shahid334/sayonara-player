#ifndef SORTING_H
#define SORTING_H

#include <QStringList>


namespace Sort {

    enum SortOrder{
        NoSorting=0,
        ArtistNameAsc,
        ArtistNameDesc,
        ArtistTrackcountAsc,
        ArtistTrackcountDesc,
        AlbumNameAsc,
        AlbumNameDesc,
        AlbumYearAsc,
        AlbumYearDesc,
        AlbumTracksAsc,
        AlbumTracksDesc,
        AlbumDurationAsc,
        AlbumDurationDesc,
        AlbumRatingAsc,
        AlbumRatingDesc,
        TrackNumAsc,
        TrackNumDesc,
        TrackTitleAsc,
        TrackTitleDesc,
        TrackAlbumAsc,
        TrackAlbumDesc,
        TrackArtistAsc,
        TrackArtistDesc,
        TrackYearAsc,
        TrackYearDesc,
        TrackLenghtAsc,
        TrackLengthDesc,
        TrackBitrateAsc,
        TrackBitrateDesc,
        TrackSizeAsc,
        TrackSizeDesc,
        TrackDiscnumberAsc,
        TrackDiscnumberDesc,
        TrackRatingAsc,
        TrackRatingDesc
    };

}


class LibSortOrder {

public:
    Sort::SortOrder so_albums;
    Sort::SortOrder so_artists;
    Sort::SortOrder so_tracks;

    LibSortOrder(){
        so_artists = Sort::ArtistNameAsc;
        so_albums = Sort::AlbumNameAsc;
        so_tracks = Sort::TrackAlbumAsc;
    }

    LibSortOrder(const LibSortOrder& so){
        so_albums = so.so_albums;
        so_artists = so.so_artists;
        so_tracks = so.so_tracks;
    }

    bool operator==(LibSortOrder so){
        return  (so.so_albums == so_albums) &&
                (so.so_artists == so_artists) &&
                (so.so_tracks == so_tracks);
    }

    QString toString() const{
        return QString::number((int) so_albums) + "," +
                QString::number((int) so_artists) + "," +
                QString::number((int) so_tracks);
    }

    static LibSortOrder fromString(const QString str){

        LibSortOrder so;
        QStringList lst = str.split(",");
        so.so_albums = (Sort::SortOrder) lst[0].toInt();
        so.so_artists = (Sort::SortOrder) lst[1].toInt();
        so.so_tracks = (Sort::SortOrder) lst[2].toInt();
        return so;
    }
};


#endif // SORTING_H
