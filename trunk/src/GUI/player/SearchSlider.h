/* SearchSlider.h

 * Copyright (C) 2012  
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras, 
 * Sep 14, 2012 
 *
 */

#ifndef SEARCHSLIDER_H_
#define SEARCHSLIDER_H_

#include <QWidget>
#include <QSlider>

class SearchSlider: public QSlider {

	Q_OBJECT

	signals:
	void searchSliderPressed(int);
	void searchSliderReleased(int);
	void searchSliderMoved(int);


public:
	SearchSlider(QWidget* parent=0);
	virtual ~SearchSlider();

	bool isSearching();



protected:
	virtual bool event(QEvent* e);

private:
	bool	_searching;



};

#endif /* SEARCHSLIDER_H_ */
