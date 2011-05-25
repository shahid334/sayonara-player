#include "DatabaseAccess/CDatabaseConnector.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Equalizer_presets.h"
#include <vector>
#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <stdlib.h>
#include <QVariant>
#include <QObject>
#include <QSqlError>
#include <CSettingsStorage.h>


bool CDatabaseConnector::load_settings(){
	 this -> load_settings_lastfm();
	 this -> load_settings_eq();
	 return true;
}

bool CDatabaseConnector::store_settings(){
	if(!m_database.isOpen()) m_database.open();
	this->store_settings_lastfm();
	this->store_settings_eq();
	return true;
}


void CDatabaseConnector::load_settings_lastfm() {
    if (!this -> m_database.isOpen())
        this -> m_database.open();
    QString username, password;
    try {
        QSqlQuery q (this -> m_database);
        q.prepare("select value from settings where key ='LastFM_login';");
        if (!q.exec()) {
            throw QString ("SQL - Error: fillSettingsStorage");
        }

		if(q.next()){
			QStringList list = q.value(0).toString().split(',');
			if(list.size() == 2){
				username = list[0];
				password = list[1];
			}
		}

		CSettingsStorage::getInstance()->setLastFMNameAndPW(username,password);
    }

    catch (QString ex) {
        qDebug() << "Error during inserting of metadata into database";
        qDebug() << ex;
    }
}


void CDatabaseConnector::store_settings_lastfm () {

    try {
        QString username, password;
        CSettingsStorage::getInstance()->getLastFMNameAndPW(username,password);
        QSqlQuery q (this -> m_database);
        q.prepare("select value from settings where key = 'LastFM_login'");
        if (!q.exec()) {
            throw QString ("SQL - Error: storeSettingsFromStorage");
        }
        if (!q.next()) {
            q.prepare("update settings set value='dummy,dummy' WHERE key='LastFM_login'");
            if (!q.exec()) {
                throw QString ("SQL - Error: storeSettingsFromStorage");
            }
        }
        q.prepare("UPDATE settings set value=? WHERE key='LastFM_login'");
        q.addBindValue(QVariant(username+","+password));

        if (!q.exec()) {
            throw QString ("SQL - Error: storeSettingsFromStorage");
        }
    }
    catch (QString ex) {
        qDebug() << "Error during inserting of metadata into database";
        qDebug() << ex;
        QSqlError er = this -> m_database.lastError();
        qDebug() << er.driverText();
        qDebug() << er.databaseText();
        qDebug() << er.databaseText();
    }
}


void CDatabaseConnector::load_settings_eq(){

	if (!this -> m_database.isOpen())
	        this -> m_database.open();

	vector<EQ_Setting> result;
    try{
		QSqlQuery q (this -> m_database);
		q.prepare("select value from settings where key like 'EQ_pr_%';");

		if (!q.exec()) {
			throw QString (q.lastError().text());
		}

		while (q.next()) {
		   EQ_Setting eq_setting;
		   QString row = q.value(0).toString();
		   eq_setting.parseFromString(row);
		   result.push_back(eq_setting);
		}

		CSettingsStorage::getInstance()->setEqualizerSettings(result);
    }
    catch(QString ex){
    	qDebug() << "Error fetching EQ Presets (" << ex << ")";
    }
}




void CDatabaseConnector::store_settings_eq(){

	 try {

	        vector<EQ_Setting> vec;
	        CSettingsStorage::getInstance()->getEqualizerSettings(vec);
	        QString str2insert = "";
	        for(uint i=0; i<vec.size(); i++){
	        	if(vec[i].name == "Custom"){
	        		str2insert = vec[i].toString();
	        		break;
	        	}
	        }

	        QSqlQuery q (this -> m_database);
	        q.prepare("select value from settings where key = 'EQ_pr_custom'");

	        if (!q.exec()) {
	            throw QString ("SQL - Error: store settings equalizer (database not ready)");
	        }

	        if (!q.next() || str2insert == "") {
	            q.prepare("insert into settings  values('EQ_pr_custom', ?);");
	            q.addBindValue(str2insert);

	            if (!q.exec()) {
	                throw QString ("SQL - Error: storeSettingsFromStorage");
	            }

	            return;
	        }

	        if(str2insert != ""){
				q.prepare("UPDATE settings set value=? WHERE key='EQ_pr_custom'");
				q.addBindValue(str2insert);

				if (!q.exec()) {
					throw QString ("SQL - Error: storeSettingsFromStorage");
				}
				else{
					qDebug() << "inserted successfully";
				}
	        }
	    }

	    catch (QString ex) {
	        qDebug() << "Error during inserting of equalizer into database";
	        qDebug() << ex;
	    }
}
