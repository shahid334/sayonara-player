/*
 *
 * valid_document = first tag LIKE <rss version="2.0">
 * valid_item = channel.item.enclosore::(type).contains(audio) > channel.item.enclosure == NULL
 *
 *
 * MetaData.title = channel.item.title
 * MetaData.artist = itunes:author > channel.item.author
 * MetaData.album = channel.title
 * MetaData.year = channel.item.pubDate.year > channel.pubDate.yeicon.url = > item.image.url > itunes:image (1400x1400) px 
 *
 *
 *
 * */
#include <QString>
#include <QDomDocument>
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"

#include "HelperStructs/PodcastParser/PodcastParser.h"

#include <QDebug>


bool Podcast::parse_podcast_xml_file(QString podcast_filename, MetaDataList& v_md){


    QString content;

    if(podcast_filename.startsWith("http"))
        Helper::read_http_into_str(podcast_filename, content);
    else
        Helper::read_file_into_str(podcast_filename, content);

    v_md.clear();

    QDomDocument doc("PodcastFile");
    doc.setContent(content);

    QDomElement docElement = doc.documentElement();
    QDomNode entry = docElement.firstChildElement("channel");

    qDebug() << "entry has child nodes: " << entry.childNodes().size();

    if(!entry.hasChildNodes()) return false;



    for(int c = 0; c<entry.childNodes().size(); c++){
        MetaData md;

        QDomNode channel_child = entry.childNodes().at(c);
        QString nodename = channel_child.nodeName();
        QDomElement e = channel_child.toElement();

        qDebug() << "Channel nodename = " << nodename;

        if(!nodename.compare("title")){
            md.album = e.text();
        }

        else if(!nodename.compare("itunes:category")){

            QStringList genres = e.text().split(QRegExp(",|/|;|\\."));
            for(int i=0; i<genres.size(); i++){
                genres[i] = genres[i].trimmed();
            }

            md.genres.push_back("Podcasts");
            md.genres.append(genres);
        }



        else if(!nodename.compare("item")){

            qDebug() << "item " << nodename << " has childnodes? " << channel_child.hasChildNodes();
            if(!channel_child.hasChildNodes()) continue;

            for(int i=0; i<channel_child.childNodes().size(); i++)
            {

                QDomNode item_child = channel_child.childNodes().at(i);
                QString ic_nodename = item_child.nodeName();
                QDomElement ic_e = item_child.toElement();

                qDebug() << "nodename = " << ic_nodename;

                if(!ic_nodename.compare("title")){

                    md.title = ic_e.text();
                    qDebug() << "found title: " << md.title;
                }

                else if(!ic_nodename.compare("link")){

                    md.filepath = ic_e.text();
                    qDebug() << "found url: " << md.filepath;
                }

                else if(!ic_nodename.compare("enclosure")){

                    md.filesize = ic_e.attribute("length").toLong();
                    qDebug() << "found enclosure: " << md.filesize;
                }

                else if(!ic_nodename.compare("author") && md.artist.size() == 0){
                    md.artist = ic_e.text();
                    qDebug() << "found author: " << md.artist;
                }

                else if(!ic_nodename.compare("itunes:author")){
                    md.artist = ic_e.text();
                    qDebug() << "found better author: " << md.artist;
                }

                else if(!ic_nodename.compare("itunes:duration")){


                    QString text = ic_e.text();
                    qDebug() << "found durationr: " << text;
                    QStringList lst = text.split(":");
                    int len = 0;
                    for(int i=lst.size() -1; i>=0; i--){
                        if(i == lst.size() -1)
                            len += lst[i].toInt();
                        else if(i == lst.size() -2){
                            len += lst[i].toInt() * 60;
                        }
                        else if(i == lst.size() -3){
                            len += lst[i].toInt() * 3600;
                        }
                    }

                    qDebug() << "found lenght: " << len;

                    md.length_ms = len * 1000;
                }

            } // foreach item entry
        } // item

        if(md.filepath.size() > 0)
            v_md.push_back(md);
        qDebug() << "next ";
        qDebug() << "";
    }

    return (v_md.size() > 0);

}
