/* EqSlider.h */

/* Copyright (C) 2011-2015  Lucio Carreras
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



#ifndef EQSLIDER_H
#define EQSLIDER_H

#include <QSlider>
#include <QLabel>
#include <QEvent>

class EqSlider : public QSlider {
	Q_OBJECT

	signals:
		void sig_value_changed(int idx, int val);
		void sig_slider_got_focus(int idx);
		void sig_slider_lost_focus(int idx);

	public:
		EqSlider(QWidget* parent);
		virtual ~EqSlider() {}

		void setData(int idx, QLabel* label);
		QLabel* getLabel();
		int getIndex();
		void setIndex(int idx);


	protected:
		virtual void sliderChange(SliderChange change);

	private:

		QLabel* _label;
		int _idx;

		void focusInEvent(QFocusEvent* e);
		void focusOutEvent(QFocusEvent* e);
		bool event(QEvent *e);
};

#endif // EQSLIDER_H
