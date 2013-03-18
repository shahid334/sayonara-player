#include "StreamPlugins/LastFM/LFMLoginThread.h"
#include "StreamPlugins/LastFM/LFMGlobals.h"
#include <StreamPlugins/LastFM/LFMWebAccess.h>
#include "HelperStructs/Helper.h"
#include <QString>
#include <QCryptographicHash>
#include <QMessageBox>


LFMLoginThread::LFMLoginThread(QObject *parent) :
    QThread(parent)
{
}

LFMLoginThread::~LFMLoginThread(){}

bool LFMLoginThread::get_token(){

    QString token;
    UrlParams signature_data;
        signature_data["api_key"] = LFM_API_KEY;
        signature_data["method"] = "auth.gettoken";

    QString url = lfm_wa_create_sig_url(QString("http://ws.audioscrobbler.com/2.0/"), signature_data);

    QString response;
    bool success = lfm_wa_call_url(url, response);

    if(!success){
        qDebug() << "LFM: could not call login url " << url;
        _login_info.logged_in = false;
        return false;
    }


    token = Helper::easy_tag_finder("lfm.token", response);
    if(token.size() != 32) return false;
    qDebug() << "Token = " << token;
    _login_info.token = token;
    return true;
}

bool LFMLoginThread::request_authorization(){


    if(!get_token()) return false;


    UrlParams signature_data;
        signature_data["api_key"] = LFM_API_KEY;
        signature_data["token"] = _login_info.token;

        qDebug() << "auth: token = " << _login_info.token;

    QString url = lfm_wa_create_std_url("http://www.last.fm/api/auth/", signature_data);
        int ret = QMessageBox::question(NULL,
                                 "Last.fm request authorization",
                                 QString("<b>First login to Last.fm from Sayonara</b><br /><br />") +
                                 "You will be redirected to this link when clicking on it<br /><br />" +
                                 "<a href=\"" + url + "\">"+ url + "</a><br /><br />" +
                                 "When you finished authorization, click OK", QMessageBox::Ok, QMessageBox::Abort);

    if(ret == QMessageBox::Abort) return false;
    if(ret == QMessageBox::Ok) return true;
    return false;
}



void LFMLoginThread::run(){

    _login_info.logged_in = false;

    UrlParams signature_data;
        signature_data["api_key"] = LFM_API_KEY;
        signature_data["method"] = "auth.getMobileSession";
        signature_data["password"] = _password;
        signature_data["username"] = _username;


    string post_data;
    QString url = lfm_wa_create_sig_url_post("https://ws.audioscrobbler.com/2.0/", signature_data, post_data);
    QString response;

    qDebug() << "url = " << url;

    bool success = lfm_wa_call_post_url_https(url, post_data, response);
    if(!success){
        qDebug() << "get session: no success!";
        qDebug() << response;
        _login_info.logged_in = false;
        _login_info.session_key = "";
        _login_info.subscriber = false;

    }

    else {

        _login_info.logged_in = true;
        _login_info.session_key = Helper::easy_tag_finder("lfm.session.key", response);
        _login_info.subscriber = (Helper::easy_tag_finder("lfm.session.subscriber", response).toInt() == 1);
        _login_info.error = response;
    }
}


void LFMLoginThread::setup_login_thread(QString username, QString password){

    _username = username;
    _password = password;
}

LFMLoginStuff LFMLoginThread::getLoginStuff(){
    return _login_info;
}
