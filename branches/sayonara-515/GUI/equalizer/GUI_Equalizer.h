/* GUI_Equalizer.h */

/* Copyright (C) 2011  Lucio Carreras
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


/*
 * GUI_Equalizer.h
 *
 *  Created on: May 18, 2011
 *      Author: luke
 */

#ifndef GUI_EQUALIZER_H_
#define GUI_EQUALIZER_H_

#include "HelperStructs/Equalizer_presets.h"
#include "ui_GUI_Equalizer.h"

#include <QObject>
#include <vector>

using namespace std;

class GUI_Equalizer : public PlayerPlugin, private Ui::GUI_Equalizer{

	Q_OBJECT

public:
	GUI_Equalizer(QString name, QString action_name, QChar shortcut, QWidget* parent=0);
	virtual ~GUI_Equalizer();

	signals:
		void eq_changed_signal(int, int);
		void eq_enabled_signal(bool);
		void eq_changed_level_signal(int);
		void close_event();


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

		void but_enabled_changed(bool);
		void preset_changed(int);
		void btn_preset_clicked();

	public slots:
		void fill_eq_presets(const vector<EQ_Setting>&);
		void fill_available_equalizers(const QStringList&);
        void changeSkin(bool);

	private:
        Ui::GUI_Equalizer* _ui;
		vector<EQ_Setting> _presets;

        double _m;
        double _t;

        bool _dark;
};

#endif /* GUI_EQUALIZER_H_ */
