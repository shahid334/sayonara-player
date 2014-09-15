/* LFMLoginThread.h */

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



#ifndef LFMLOGINTHREAD_H
#define LFMLOGINTHREAD_H

#include <QThread>

#include <QString>
#include "HelperStructs/globals.h"

struct LFMLoginStuff{
    QString token;
    QString session_key;
    bool logged_in;
    bool subscriber;
    QString error;
};

class LFMLoginThread : public QThread
{
    Q_OBJECT

public:
    LFMLoginThread(QObject *parent = 0);
    virtual ~LFMLoginThread();
    
protected:
    void run();

public:
    void setup_login_thread(QString username, QString password);
    LFMLoginStuff getLoginStuff();
    bool get_token();
    bool request_authorization();


private:
    QString _username;
    QString _password;

    LFMLoginStuff _login_info;


};



#endif // LFMLOGINTHREAD_H
