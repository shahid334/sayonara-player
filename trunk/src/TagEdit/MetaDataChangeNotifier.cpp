#include "TagEdit/MetaDataChangeNotifier.h"

MetaDataChangeNotifier::MetaDataChangeNotifier(QObject *parent) :
	QObject(parent)
{
}

MetaDataChangeNotifier::~MetaDataChangeNotifier(){

}


void MetaDataChangeNotifier::change_metadata(const MetaDataList& v_md_old, const MetaDataList& v_md_new){

	emit sig_metadata_changed(v_md_old, v_md_new);
}
