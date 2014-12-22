#include "TagEdit/TagExpression.h"
#include "HelperStructs/Helper.h"
#include <QRegExp>
#include <QMap>
#include <QDebug>

TagExpression::TagExpression(){
	_tag_regex_map.insert(TAG_TITLE, QString("(.+)"));
	_tag_regex_map.insert(TAG_ALBUM, QString ("(.+)"));
	_tag_regex_map.insert(TAG_ARTIST, QString("(.+)"));
	_tag_regex_map.insert(TAG_TRACK_NUM, QString("(\\d+)"));
	_tag_regex_map.insert(TAG_YEAR, QString("(\\d{4})"));
	_tag_regex_map.insert(TAG_DISC, QString("(\\d{1,2})"));
}

TagExpression::TagExpression(const QString& tag_str, const QString& filepath) :
	TagExpression()
{
	update_tag(tag_str, filepath);
}

TagExpression::~TagExpression(){

}


QString TagExpression::escape_special_chars(const QString& str){

	QString s = str;
	QStringList str2escape;
	str2escape << "\\" << "?" << "+" << "*" << "[" << "]" << "(" << ")" << "{" << "}" << "."; //<< "-";

	foreach(QString c, str2escape){
		s.replace(c, QString("\\") + c);
	}

	return s;
}

QStringList TagExpression::split_tag_string( const QString& tag_str ){

	QStringList splitted_tag_str;
	QString tag_str_tmp = escape_special_chars(tag_str);

	QMap<int, QString> tags;

	QStringList available_tags;
	available_tags  << TAG_TITLE
					<< TAG_ALBUM
					<< TAG_ARTIST
					<< TAG_TRACK_NUM
					<< TAG_YEAR
					<< TAG_DISC;

	foreach(QString tag, available_tags){
		int idx = tag_str_tmp.indexOf(tag);
		if(idx >= 0){
			tags.insert(idx, tag);
		}
	}

	int cur_idx = 0;

	foreach(int idx, tags.keys()){

		int len = idx - cur_idx;

		splitted_tag_str << tag_str_tmp.mid(cur_idx, idx - cur_idx);
		splitted_tag_str << tags[idx];

		cur_idx += (tags[idx].size() + len);
	}

	splitted_tag_str << tag_str_tmp.right(tag_str_tmp.length() - cur_idx);
	splitted_tag_str.removeAll("");

	return splitted_tag_str;
}



QString TagExpression::calc_regex_string(const QStringList& splitted_str){

	QString re_str;

	foreach(QString s, splitted_str){

		if(s.isEmpty()) continue;


		if( _tag_regex_map.keys().contains(s) ){
			re_str += _tag_regex_map[s];
		}

		else{
			re_str += "(" + s + ")";
		}
	}

	return re_str;
}



void TagExpression::update_tag(QString tag_str, QString filepath){

	if(tag_str == _tag_str && filepath == _filepath) return;

	_cap_map.clear();

	QStringList captured_texts;
	QStringList splitted_tag_str = split_tag_string(tag_str);
	QString regex = calc_regex_string(splitted_tag_str);

	QRegExp re( regex );

	int n_tags;
	int n_caps;

	re.indexIn( filepath );

	captured_texts = re.capturedTexts();
	captured_texts.removeAt(0);
	captured_texts.removeAll("");

	n_caps = captured_texts.size();
	n_tags = splitted_tag_str.size();

	_valid = (n_caps == n_tags);

	if( !_valid ){

		qDebug() << regex;
		qDebug() << "Warning: " << n_caps << " tags found, but requested " << n_tags;
		qDebug() << "Caps: ";
		qDebug() << "";

		foreach(QString s, captured_texts){
			qDebug() << "  " << s;
		}

		qDebug() << "";
	}

	else{

		for(int i=0; i<n_caps; i++){

			QString tag = splitted_tag_str[i];
			QString cap = captured_texts[i];

			if(i==0){
				QString dir, filename;
				Helper::split_filename(cap, dir, filename);
				cap = filename;
			}


			_cap_map[tag] = cap;
		}
	}
}

bool TagExpression::check_tag(QString tag, QString str){
	if(!_tag_regex_map.keys().contains(tag)) return false;
	str = escape_special_chars(str);

	QRegExp re(_tag_regex_map[tag]);

	if(re.indexIn(str) != 0 ) return false;

	return true;
}

QMap<QString, QString> TagExpression::get_tag_val_map(){
	return _cap_map;
}


bool TagExpression::is_valid(){
	return _valid;
}
