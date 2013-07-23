#ifndef GUI_PLAYLISTENTRY_H
#define GUI_PLAYLISTENTRY_H

#include <QObject>
#include <QFrame>
#include <QWidget>
#include "HelperStructs/MetaData.h"

class GUI_PlaylistEntry : public QFrame {

    Q_OBJECT

public:
    GUI_PlaylistEntry(QWidget* parent){}
    virtual ~GUI_PlaylistEntry(){}

    virtual  void setContent(const MetaData& md, int idx)=0;
    void setWidth(int w){
        this->resize(w, this->height());
    }
};


#endif // GUI_PLAYLISTENTRY_H
