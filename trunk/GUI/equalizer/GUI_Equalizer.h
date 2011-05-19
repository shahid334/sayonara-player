/*
 * GUI_Equalizer.h
 *
 *  Created on: May 18, 2011
 *      Author: luke
 */

#ifndef GUI_EQUALIZER_H_
#define GUI_EQUALIZER_H_

#include <QObject>
#include <QWidget>
#include <ui_GUI_equalizer.h>

class GUI_Equalizer : public QWidget, private Ui_GUI_Equalizer{

	Q_OBJECT

public:
	GUI_Equalizer(QWidget* parent=0);
	virtual ~GUI_Equalizer();

	signals:
		void eq_changed_signal(int, int);



	private slots:
		void sli_0_changed(int);
		void sli_1_changed(int);
		void sli_2_changed(int);
		void sli_3_changed(int);
		void sli_4_changed(int);
		void sli_5_changed(int);
		void sli_6_changed(int);
		void sli_7_changed(int);
		void sli_8_changed(int);
		void sli_9_changed(int);

	private:
		Ui_GUI_Equalizer* _ui;
};

#endif /* GUI_EQUALIZER_H_ */
