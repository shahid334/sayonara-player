/* SmartComparison.cpp */

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



#include "SmartComparison.h"

SmartComparison::SmartComparison()
{

	CDatabaseConnector::getInstance()->getAllArtists(_artists);
	_vocals << 'a' << 'e' << 'i' << 'o' << 'u';
	_consonants << 'b' << 'c' << 'd' << 'f' <<
				   'g' << 'h' << 'j' << 'k' <<
				   'l' << 'm' << 'n' << 'p' <<
				   'q' << 'r' << 's' << 't' <<
				   'v' << 'w' << 'x' << 'y' << 'z';
				   'z';

	_numbers << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8' << '9' << '0';
}

QString revert_string(const QString& str){

	QString ret = "";
	if(str.size() == 0) return "";

	for(int i=str.size()-1; i>=0; i--){
		ret.append(str[i]);
	}

	return ret;
}


float SmartComparison::compare_hashes(const Hash& str1, const Hash& str2, HashLevel level, bool reverse){

	QString s1 = str1.hash;
	QString s2 = str2.hash;

	if(reverse){
		s1 = revert_string(str1.hash);
		s2 = revert_string(str2.hash);
	}

	int min_str_size = min(str1.org_str.size(), str2.org_str.size());
	if(min_str_size == 0) return 0;

	int min_hash_size = min(str1.hash.size(), str2.hash.size());
	int max_hash_size = max(str1.hash.size(), str2.hash.size());
	if(min_hash_size == 0) return 0;

	float similarity = 0;

	int bad_letters = 0;
	float rate_bad_letters[2];

	int right_letters = 0;
	float rate_right_letters[2];

	int consec_letters = 0;
	float rate_consec_letters[2];

	int idx1 = 0;
	int idx2 = 0;

	float rate = 1.0f;

	QMap<int, int> pairs;

	while(true){

		QChar ncl = s1.at(idx1);
		bool found = false;
		int wrong2letters=0;

		for(int i=idx2; i<s2.size(); i++){
			QChar c = s2.at(i);
			if(c == ncl){
				found = true;
				pairs.insert(idx1, idx2);

				if(i == idx2){
					consec_letters++;
					rate *= 1.5f;
				}


				right_letters++;

				idx2 = i;
				break;
			}


			if(i==idx2 && rate > 1.0f){
				rate /= 1.5f;
			}

			wrong2letters++;
		}

		if(!found){
			bad_letters++;
		}

		else {
			bad_letters += wrong2letters;
			idx2 ++;
		}

		idx1++;

		if(idx1 == s1.size()) break;
		if(idx2 == s2.size()) break;
	}


	bad_letters += s1.size() - idx1;
	bad_letters += s2.size() - idx2;

	rate_consec_letters[0] = (consec_letters * 1.0f) / (min_hash_size * 1.0f);
	rate_right_letters[0] = (right_letters * 1.0f) / (min_hash_size * 1.0f);
	rate_bad_letters[0] = (bad_letters * 1.0f) / (min_hash_size * 1.0f);

	rate_consec_letters[1] = (consec_letters * 1.0f) / (max_hash_size * 1.0f);
	rate_right_letters[1] = (right_letters * 1.0f) / (max_hash_size * 1.0f);
	rate_bad_letters[1] = (bad_letters * 1.0f) / (max_hash_size * 1.0f);


	similarity = 0;
	for(int i=0; i<2; i++){
		similarity += (1.0f - rate_bad_letters[i]) * 5.0f;
		similarity += rate_right_letters[i] * 1.0f;
		similarity += rate_consec_letters[i] * 10.0f * rate;
	}

	similarity /= (32.0f);
	//similarity *= similarity;

//	similarity = pow(similarity, str1.penalty);

	/*if(bad_letters < right_letters || bad_letters < consec_letters){
		qDebug() << "";
		str1.print();
		str2.print();
		qDebug() << "Idx1 = " << idx1 << "/" << s1.size() << ", idx2 = " << idx2 << "/" << s2.size();
		qDebug() << "Bad Letters: " << bad_letters;
		qDebug() << "Right letters: " <<  right_letters;
		qDebug() << "Conseq letter: " << consec_letters;
		qDebug() << "";
	}

	else return 0;*/

	return similarity;
}



float SmartComparison::compare(const QString& str1, const QString& str2, HashLevel level, bool revert){

	Hash hash1 = create_hash(str1, level);
	Hash hash2 = create_hash(str2, level);

	float similarity = compare_hashes(hash1, hash2, level, revert);
	float similarity2 = compare_hashes(hash2, hash1, level, revert);

	return max(similarity,similarity2);
}

Hash SmartComparison::create_hash(const QString& str, HashLevel level){

	Hash hash;

	hash.org_str = str;
	hash.hash = str.toLower();

	if(level >= HashLevel_zero){
		hash.penalty = 1.0f;

		hash.hash = remove_special_chars(hash.hash);
	}

	if(level > HashLevel_zero){
		hash.penalty = 5.0f;
		hash.hash = remove_vocals(hash.hash);

	}

	else if(level > HashLevel_medium){
		hash.penalty = 8.0f;
		hash.hash = sort_letters(hash.hash);
	}


	return hash;

}

void SmartComparison::print_similar(const QString& str){

	QMap<QString, float> similar = get_similar_strings(str);
	foreach(QString key, similar.keys()){

		float val = similar.value(key);

		qDebug() << str << "?     " << key << ": " << val;
	}
}

void SmartComparison::equalize(QMap<QString, float>& map, float min, float max){

	float stretch = 1.0f / (max - min);
	QStringList to_remove;

	foreach(QString key, map.keys()){
		float val = map.value(key);

		val = (val - min) * stretch;
		if(val > 0.5f){
			map[key] = val;
		}

		else{
			to_remove << key;
		}
	}

	foreach(QString str, to_remove){
		map.remove(str);
	}
}

QMap<QString, float> SmartComparison::get_similar_strings( const QString& str){

	HashLevel level = HashLevel_zero;

	bool reverse = false;

	while(true){

		float max = std::numeric_limits<float>::min();
		float min = std::numeric_limits<float>::max();

		QMap<QString, float> map;

		foreach(Artist artist, _artists){

			float similarity = compare(artist.name, str, level, false);
			similarity = std::max(similarity, compare(artist.name, str, level, true));

			if(similarity > max){
				max = similarity;
			}

			if(similarity < min){
				min = similarity;
			}

			if(similarity > 1.0f){
				map.insert(artist.name, similarity);
			}
		}

		if( max <  1.0f && reverse == false){
			reverse = true;
			continue;
		}


		else if(max < 1.0f && level < HashLevel_strong){
			if(level == HashLevel_zero) level = HashLevel_medium;
			if(level == HashLevel_medium) level = HashLevel_strong;
			reverse = false;

			continue;
		}

		else {
			//qDebug() << "LEvel : " << level;
			equalize(map, min, max);
			return map;
		}
	}
}

QString SmartComparison::remove_special_chars(const QString& str){

	QString ret = str;

	foreach(QChar c, ret){
		if(_vocals.contains(c)) continue;
		if(_consonants.contains(c)) continue;
		if(_numbers.contains(c)) continue;

		ret = ret.remove(c);
	}

	return ret;
}

QString SmartComparison::remove_vocals(const QString& str){

	QString ret = str;
	foreach(QString vocal, _vocals){
		ret.remove(vocal);
	}

	return ret;
}

QString SmartComparison::sort_letters(const QString& str){

	QString ret = "";
	QMap<QChar, int> cons_count;
	QMap<QChar, int> vocal_count;
	QMap<QChar, int> number_count;

	foreach(QChar c, str){

		if(_vocals.contains(c)){
			if( vocal_count.contains(c) ){
				int count = vocal_count.value(c);
				count += 1;
				vocal_count[c] = count;
			}

			else{
				vocal_count.insert(c, 1);
			}
		}

		else if(_consonants.contains(c)){
			if( cons_count.contains(c) ){
				int count = cons_count.value(c);
				count += 1;
				cons_count[c] = count;
			}

			else{
				cons_count.insert(c, 1);
			}
		}

		else if(_numbers.contains(c)){
			if( number_count.contains(c) ){
				int count = number_count.value(c);
				count += 1;
				number_count[c] = count;
			}

			else{
				number_count.insert(c, 1);
			}
		}
	}

	foreach(QChar c, vocal_count.keys()){
		int count = vocal_count[c];

		for(int i=0; i<count; i++){
			ret += c;
		}
	}

	foreach(QChar c, cons_count.keys()){
		int count = cons_count[c];

		for(int i=0; i<count; i++){
			ret += c;
		}
	}

	foreach(QChar c, number_count.keys()){
		int count = number_count[c];

		for(int i=0; i<count; i++){
			ret += c;
		}
	}

	return ret;
}
