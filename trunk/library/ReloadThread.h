/*
 * ReloadThread.h
 *
 *  Created on: Jun 19, 2011
 *      Author: luke
 */

#ifndef RELOADTHREAD_H_
#define RELOADTHREAD_H_


#include <QThread>
#include <QObject>
#include <QString>
#include <HelperStructs/CDirectoryReader.h>
#include "HelperStructs/MetaData.h"
#include <vector>

using namespace std;

class ReloadThread : public QThread {

Q_OBJECT

signals:

	void reloading_library(int);


public:
	ReloadThread();
	virtual ~ReloadThread();

	void set_lib_path(QString library_path);
	void get_metadata(vector<MetaData>&);
	int getState();


protected:
    void run();

private:
    int		_state;
    QString _library_path;
    vector<MetaData> _v_metadata;
};

#endif /* RELOADTHREAD_H_ */
