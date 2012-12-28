#ifndef GUI_ALTERNATECOV_H
#define GUI_ALTERNATECOV_H

#include <QWidget>
#include <QString>

class GUI_AlternateCov : QWidget {



public:
    GUI_AlternateCov(QWidget* parent=0) : QWidget(parent){

    }

signals:
    void cover_clicked(QString filename);

public slots:
    void cover_clicked(int row, int col){

    }

    void start(QString searchstring, QString target_filename);
    void start(int album_artist_id, bool search_for_album);


protected:
    bool _no_cover;
    QString _class_name;


};




#endif // GUI_ALTERNATECOV_H
