
#ifndef GUI_TAGEDIT_H_
#define GUI_TAGEDIT_H_

#include <QString>
#include <QRegExp>

#include "HelperStructs/SayonaraClass.h"
#include "TagEdit/TagExpression.h"
#include "TagEdit/TagEdit.h"
#include "GUI/ui_GUI_TagEdit.h"


class GUI_TagEdit : public SayonaraWidget, private Ui::GUI_TagEdit {

	Q_OBJECT
public:
	GUI_TagEdit(TagEdit* tag_edit, QWidget* parent=0);
	virtual ~GUI_TagEdit();

signals:
	void sig_ok_clicked(const MetaDataList&);
	void sig_undo_clicked(int idx);
	void sig_undo_all_clicked();


private slots:
	void next_button_clicked();
	void prev_button_clicked();
	void apply_tag_clicked();
	void apply_tag_all_clicked();

	void album_all_changed(bool);
	void artist_all_changed(bool);
	void genre_all_changed(bool);
	void year_all_changed(bool);
	void discnumber_all_changed(bool);

	void tag_selection_changed();

	void btn_title_checked(bool b);
	void btn_artist_checked(bool b);
	void btn_album_checked(bool b);
	void btn_track_nr_checked(bool b);
	void btn_disc_nr_checked(bool b);
	void btn_year_checked(bool b);

	void tag_text_changed(const QString&);
	void set_tag_colors(bool valid);
	void ok_button_clicked();

	void undo_clicked();
	void undo_all_clicked();

	void progress_changed(int);
	void metadata_changed(const MetaDataList&);




private:
	TagEdit* _tag_edit;
	TagExpression _tag_expression;

	int _cur_idx;

	QString _album_all;
	QString _artist_all;
	QString _genre_all;

	int _discnumber_all;
	int _year_all;

	int _tag_selection_start;
	int _tag_selection_len;
	QMap<QString, QString> _tag_str_map;


	bool replace_selected_tag_text(QString t, bool b);
	void apply_tag(int idx);

	void track_idx_changed();
	void reset();
	void write_changes(int idx);
	bool check_tag(int idx, const QString& str);
	bool check_idx(int idx);
};

#endif
