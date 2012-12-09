#include "StreamPlugins/LastFM/LFMLoginThread.h"
#include "StreamPlugins/LastFM/LFMGlobals.h"
#include <StreamPlugins/LastFM/LFMWebAccess.h>
#include <QString>
#include <QCryptographicHash>


LFMLoginThread::LFMLoginThread(QObject *parent) :
    QThread(parent)
{
}

LFMLoginThread::~LFMLoginThread(){}



void LFMLoginThread::run(){

    _login_info.logged_in = false;

    _login_info.authToken = QCryptographicHash::hash(_username.toUtf8() + _password.toUtf8(), QCryptographicHash::Md5).toHex();

    UrlParams signature_data;
        signature_data["api_key"] = LFM_API_KEY;
        signature_data["authToken"] =  _login_info.authToken;
        signature_data["method"] = QString("auth.getmobilesession");
        signature_data["username"] = _username;

    QString url = lfm_wa_create_sig_url(QString("http://ws.audioscrobbler.com/2.0/"), signature_data);

    QString response;
    bool success = lfm_wa_call_url(url, response);

    if(!success){
        qDebug() << "LFM: could not call login url " << url;
        _login_info.logged_in = false;
        return;
    }

    _login_info.sessionKey = lfm_wa_parse_session_answer(response);

    if(_login_info.sessionKey.size() != 0) {
        _login_info.logged_in = true;
    }

    else{
        qDebug() << "LFM: session key not valid";
        _login_info.logged_in = false;
        return;
    }


    // only for radio
    UrlParams handshake_data;
        handshake_data["version"] = QString::number(1.5);
        handshake_data["platform"] = QString("linux");
        handshake_data["username"] = _username.toLower();
        handshake_data["passwordmd5"] = _password;

    QString url_handshake = lfm_wa_create_std_url("http://ws.audioscrobbler.com/radio/handshake.php", handshake_data);
    QString resp_handshake;

    success = lfm_wa_call_url(url_handshake, resp_handshake);
    if( !success ){
        _login_info.sessionKey2 = "";
        qDebug() << "LFM: Handshake was not successful";
        qDebug() << "LFM: url = " << url_handshake;
        qDebug() << "LFM: " << resp_handshake;
        return;
    }

    resp_handshake.replace("session=", "");
     _login_info.sessionKey2 = resp_handshake.left(32);
     qDebug() << "session key 2 = " << _login_info.sessionKey2;

}


void LFMLoginThread::setup_login_thread(QString username, QString password){

    _username = username;
    _password = password;
}

LFMLoginStuff LFMLoginThread::getLoginStuff(){
    return _login_info;
}
