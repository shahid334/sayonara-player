#ifndef _CUSTOMMIMEDATA_H
#define _CUSTOMMIMEDATA_H

#include "HelperStructs/MetaData.h"
#include <QMimeData>
#include <QDebug>


class CustomMimeData : public QMimeData {


private:
	MetaDataList _v_md;
    bool _has_meta_data;


public:
    CustomMimeData() : QMimeData(){

        _has_meta_data = false;
    }

    virtual ~CustomMimeData(){
    }

    void setMetaData(const MetaDataList& v_md){

		_v_md = v_md;
        _has_meta_data = (v_md.size() > 0);
	}

    uint getMetaData(MetaDataList& v_md) const {

        if(!_has_meta_data) return 0;
        if(_v_md.size() == 0) return 0;
		v_md = _v_md;
        return _v_md.size();
	}

    bool hasMetaData() const {

        return _has_meta_data;
    }
};


#endif
