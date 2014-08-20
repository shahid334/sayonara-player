#include "CoverLookup/CoverLookupAll.h"
#include <unistd.h>

CoverLookupAll::CoverLookupAll(QObject* parent, const AlbumList& album_list) :
    CoverLookupInterface(parent),
    _album_list(album_list),
    _run(true)
{
    _cl = new CoverLookup(this);
    connect(_cl, SIGNAL(sig_cover_found(QString)), this, SLOT(cover_found(QString)));
    connect(_cl, SIGNAL(sig_finished(bool)), this, SLOT(finished(bool)));
}


CoverLookupAll::~CoverLookupAll(){

    _cl->stop();
}


void CoverLookupAll::start(){

    Album album = _album_list.back();
    _cl->fetch_album_cover(album);
}


void CoverLookupAll::stop(){
    _run = false;
    _cl->stop();
}


void CoverLookupAll::cover_found(QString file_path){

    _album_list.pop_back();
    emit sig_cover_found(file_path);

    if(!_run) return;

    // Google and other webservices block, if looking too fast
    usleep(1000000);

    Album album = _album_list.back();
    _cl->fetch_album_cover(album);
}

void CoverLookupAll::finished(bool success){
    emit sig_finished(success);
}
