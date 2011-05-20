/*
 * GUI_Equalizer.h
 *
 *  Created on: May 18, 2011
 *      Author: luke
 */

#ifndef GUI_EQUALIZER_H_
#define GUI_EQUALIZER_H_

#include "HelperStructs/Equalizer_presets.h"

#include <QObject>
#include <QWidget>

#include <ui_GUI_equalizer.h>

#include <vector>

using namespace std;

class GUI_Equalizer : public QWidget, private Ui_GUI_Equalizer{

	Q_OBJECT

public:
	GUI_Equalizer(QWidget* parent=0);
	virtual ~GUI_Equalizer();

	signals:
		void eq_changed_signal(int, int);
		void eq_enabled_signal(bool);


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
		void cb_enabled_changed(bool);
		void preset_changed(int);

	public slots:
		void fill_eq_presets(const vector<EQ_Setting>&);

	private:
		Ui_GUI_Equalizer* _ui;
};

#endif /* GUI_EQUALIZER_H_ */
