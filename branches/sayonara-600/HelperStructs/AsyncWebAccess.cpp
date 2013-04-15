#include "HelperStructs/AsyncWebAccess.h"
#include "HelperStructs/Helper.h"



AsyncWebAccess::AsyncWebAccess(QObject* parent) : QThread(parent){
}

AsyncWebAccess::~AsyncWebAccess(){}

void AsyncWebAccess::run(){

	_data.clear();
	if(_url.size() == 0) return;
	Helper::read_http_into_str(_url, _data);
}

bool AsyncWebAccess::get_data(QString& data){
	data = _data;
	return (_data.size() > 0);
}

void AsyncWebAccess::set_url(QString url){
	_url = url;
}
