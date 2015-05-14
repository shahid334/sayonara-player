#ifndef METADATACHANGENOTIFIER_H
#define METADATACHANGENOTIFIER_H

#include "HelperStructs/globals.h"
#include "HelperStructs/MetaData/MetaData.h"

class MetaDataChangeNotifier : public QObject
{
	Q_OBJECT

	SINGLETON_QOBJECT(MetaDataChangeNotifier)


public:
	void change_metadata(const MetaDataList& v_md_old, const MetaDataList& v_md_new);

signals:
	void sig_metadata_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new);

public slots:

};

#endif // METADATACHANGENOTIFIER_H
