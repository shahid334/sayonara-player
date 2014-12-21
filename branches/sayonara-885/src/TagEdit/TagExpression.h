#ifndef TAGEXPRESSION_H
#define TAGEXPRESSION_H

#include <QMap>
#include <QString>
#include <QStringList>


#define TAG_NONE
#define TAG_TITLE QString("<t>")
#define TAG_ALBUM QString("<al>")
#define TAG_ARTIST QString("<ar>")
#define TAG_TRACK_NUM QString("<nr>")
#define TAG_YEAR QString("<y>")
#define TAG_DISC QString("<d>")

class TagExpression {


	QString _tag_str;
	QString _filepath;

	QMap<QString, QString> _cap_map;
	QMap<QString, QString> _tag_regex_map;
	bool _valid;

	QString escape_special_chars(const QString& str);
	QString calc_regex_string(const QStringList& splitted_tag_str);
	QStringList split_tag_string(const QString& tag_str);

	void update_tag(QString regex);



public:

	TagExpression();
	TagExpression(const QString& tag_str, const QString& filename);
	virtual ~TagExpression();


	void update_tag(QString tag_str, QString filename);
	bool is_valid();

	bool check_tag(QString tag, QString str);



	QMap<QString, QString> get_tag_val_map();


};

#endif // TAGEXPRESSION_H
