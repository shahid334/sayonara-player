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

#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Equalizer_presets.h"
#include "GUI/ui_GUI_Equalizer.h"


#include <QObject>
#include <vector>

using namespace std;

class EqSlider : public QObject{
	Q_OBJECT

	// QObject does not allow nested classes
	// so this is a workaround
	friend class GUI_Equalizer;

	signals:
		void valueChanged(int idx, int val);

	private:
		QSlider* _slider;
		QLabel* _label;
		int _idx;

		EqSlider(QObject* parent) : QObject(parent){}
		EqSlider(QSlider* slider, QLabel* label, int idx) :
			_slider(slider), _label(label), _idx(idx){

			connect(_slider, SIGNAL(valueChanged(int)), this, SLOT(sl_slider_changed(int)));
		}
		QSlider* getSlider(){ return _slider; }
		QLabel* getLabel(){ return _label;}
		void setValue(int val){this->_slider->setValue(val);}

	virtual ~EqSlider(){}


	private slots:
		void sl_slider_changed(int val){
			emit valueChanged(_idx, val);
		}
};



class GUI_Equalizer : public PlayerPlugin, private Ui::GUI_Equalizer{

	Q_OBJECT

public:
    GUI_Equalizer(QString name, QString action_name, QWidget* parent=0);
	virtual ~GUI_Equalizer();
    static QString getVisName(){ return tr("&Equalizer"); }
    virtual QAction* getAction();

	signals:
		void eq_changed_signal(int, int);
		void eq_enabled_signal(bool);
		void eq_changed_level_signal(int);
		void close_event();


	private slots:
		void sli_changed(int, int);

		void but_enabled_changed(bool);
		void preset_changed(int);
		void btn_preset_clicked();

	public slots:
		void fill_eq_presets();
		void fill_available_equalizers(const QStringList&);
        void changeSkin(bool);
        void language_changed();

	private:

        Ui::GUI_Equalizer* _ui;
		vector<EQ_Setting> _presets;
		CSettingsStorage* _settings;
		vector<EqSlider*> _sliders;


        double _m;
        double _t;

        bool _dark;

};

#endif /* GUI_EQUALIZER_H_ */
