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

#include "Settings/Settings.h"
#include "HelperStructs/Equalizer_presets.h"
#include "GUI/ui_GUI_Equalizer.h"

#include <QObject>
#include <QList>


class EqSlider : public QObject{
	Q_OBJECT

	// QObject does not allow nested classes
	// so this is a workaround
	friend class GUI_Equalizer;

	signals:
		void sig_value_changed(int idx, int val);
		void sig_slider_pressed(int idx);
		void sig_slider_released(int idx);

	private:
		QSlider* _slider;
		QLabel* _label;
		int _idx;

		EqSlider(QObject* parent) : QObject(parent) {}
		EqSlider(QSlider* slider, QLabel* label, int idx) :
			_slider(slider), _label(label), _idx(idx) {

			connect(_slider, SIGNAL(valueChanged(int)), this, SLOT(sl_slider_changed(int)));
			connect(_slider, SIGNAL(sliderPressed()), this, SLOT(sl_slider_pressed()));
			connect(_slider, SIGNAL(sliderReleased()), this, SLOT(sl_slider_released()));
		}
		QSlider* getSlider() { return _slider; }
		QLabel* getLabel() { return _label;}
		void setValue(int val) {this->_slider->setValue(val);}
		int getValue(){ return this->_slider->value();}

	virtual ~EqSlider() {}


	private slots:
		void sl_slider_changed(int val) {
			emit sig_value_changed(_idx, val);
		}

		void sl_slider_pressed(){
			emit sig_slider_pressed(_idx);
		}

		void sl_slider_released(){
			emit sig_slider_released(_idx);
		}
};



class GUI_Equalizer : public PlayerPlugin, private Ui::GUI_Equalizer{

	Q_OBJECT

public:
	GUI_Equalizer(QString name,QWidget* parent=0);
	virtual ~GUI_Equalizer();

	signals:
		void eq_changed_signal(int, int);

	private slots:
		void sli_changed(int, int);
		void sli_pressed(int);
		void sli_released(int);

		void preset_changed(int);
        void cb_gauss_toggled(bool);
        void btn_save_clicked();

	public slots:
		void fill_eq_presets();
        void changeSkin(bool);
        void language_changed();

	private:

		QList<EQ_Setting> _presets;
		QList<EqSlider*> _sliders;

        double _m;
        double _t;

        bool _dark;

		int _old_val[10];
		int _active_idx;

};

#endif /* GUI_EQUALIZER_H_ */
