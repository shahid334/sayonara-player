#ifndef CSettingsStorage_H
#define CSettingsStorage_H

#include <QString>
#include <QPair>
#include <vector>
#include <HelperStructs/Equalizer_presets.h>
using namespace std;

/**
  * Class to check if we are running the first time.
  * If yes we are doing bootstrapping. It is a Singleton
  */
class CSettingsStorage
{
public:
    static CSettingsStorage * getInstance();

    virtual ~CSettingsStorage ();

    /**
      * Check if sayonarra is run the first time
      * @return true if run first time
      */
    bool isRunFirstTime ();

    /**
      * Create directory .sayonara and db
      * @param deleteOld Dont care if config already exists
      * @return true if successfull
      */
    bool runFirstTime (bool deleteOld = false);


    /**
      * Returns Filename of DB as absolute path
      */
    QString getDBFileName ();




    //TODO hash
    QPair<QString,QString> getLastFMNameAndPW ();

    void getLastFMNameAndPW (QString & name, QString & pw);

    void setLastFMNameAndPW (const QString & name,const QString & pw);

    void getEqualizerSettings(vector<EQ_Setting>& vec);

    void setEqualizerSettings(const vector<EQ_Setting>& vec);


private:

    void init();
    bool m_isInit;
    CSettingsStorage() {this -> init();}
    CSettingsStorage(const CSettingsStorage&);
    CSettingsStorage& operator=(const CSettingsStorage&);

    QString m_dbFile, m_sayonaraPath, m_dbSource;
    QPair<QString,QString> m_lastFm;
    vector<EQ_Setting> m_vec_eqSettings;
};

#endif // CSettingsStorage_H
