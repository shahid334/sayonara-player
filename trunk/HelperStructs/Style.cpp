/* Style.cpp */

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
 * Style.cpp
 *
 *  Created on: Oct 27, 2011
 *      Author: luke
 */

#include "HelperStructs/Helper.h"
#include "HelperStructs/Style.h"
#include <QString>

#define NEWLINE "\n";


QString Style::get_player_back_color(){
	return QString("#383838");
}

QString Style::get_player_fore_color(){
	return QString("#D8D8D8");
}

QString Style::get_tv_style(bool dark){


	QString style = "";
			/*QString("selection-color: black; "
			"selection-background-color: #e8841a; ");			// sayonara orange*/

	if(dark) return  style + QString("border: 1px solid #282828; "
			"background-color: #2C2C2C;  "
			"alternate-background-color: #242424; "
			"color: #D8D8D8; ");

	else return style;
}


QString Style::get_tv_header_style(){

	QString header_style = QString("QHeaderView::section \{ ") +
			"background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, "  +
			"stop:0 #323232, stop: 0.5 #2E2E2E, " +
			"stop: 0.6 #2E2E2E, stop:1 #323232); " +
			"color: #D8D8D8; " +
			"padding-left: 4px; " +
			"padding-top: 5px; " +
			"padding-bottom: 5px; " +
			"border: 0px; " +
			"border-bottom: 1px solid #323232; " +
			"} ";

	return header_style;
}


QString Style::get_v_scrollbar_style(){

	QString arrow_down_filename = Helper::getIconPath() + "/arrow_down.png";
	QString arrow_up_filename = Helper::getIconPath() + "/arrow_up.png";

	QString scrollbar_style = QString("QScrollBar::vertical \{") +
			      "border: 0px solid " + get_player_back_color()  + "; " +
			      "background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, "  +
				  "stop:0 #2F2F2F, stop: 0.2 #323232, " +
				  "stop: 0.8 #323232, stop:1 #2F2F2F); " +
			      "width: 20px;" +
			      "margin: 22px 0px 22px 0px;" +
			      "border-left: 5px solid " + get_player_back_color()  + "; " +
			  "}" +

			  "QScrollBar::handle:vertical \{" +
			  "    background: #404040;" +
			  "    min-height: 20px;" +
			  "	   border: 2px solid #2C2C2C; " +
			  "    border-radius: 4px; " +
			  "}" +

			  "QScrollBar::add-line:vertical \{" +
			  "    border: 1px solid " + get_player_back_color()  + "; " +
			  "    background: " + get_player_back_color()  + "; " +
			  "    height: 20px;" +
			  "    subcontrol-position: bottom; " +
			  "    subcontrol-origin: margin; " +
			  "    padding: 2px; " +
			  "    border-left: 5px solid " + get_player_back_color()  + "; " +
			  "    image: url(" + arrow_down_filename + "); " +
			  "}" +

			  "QScrollBar::sub-line:vertical \{" +
			  "    border: 1px solid " + get_player_back_color()  + "; " +
			  "    background: " + get_player_back_color()  + "; " +
			  "    height: 20px;" +
			  "	   subcontrol-position: top;" +
			  "    subcontrol-origin: margin; " +
			  "    padding: 2px; " +
			  "    border-left: 5px solid " + get_player_back_color()  + "; " +
			  "    image: url(" + arrow_up_filename + "); " +

			  "}" +

			  "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical \{" +
			  "    border: 0px solid #3C3C3C; " +
			  "    width: 3px;" +
			  "    height: 0px;" +
			  "}" +



			  "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical \{" +
			  "    background: none;" +
			  "}";

	return scrollbar_style;
}


QString Style::get_btn_style(int intensity){
	QString style =
		QString("QPushButton") +
		"{"
		"color: white; "
		"border-width: 0px; "
		"border-radius: 7; "
		"padding: 1px; "
		"padding-left: 1px; "
		"padding-right: 1px; "
		"}"
		"QPushButton:checked"
		"\{"
		"background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.55, fx:0.502, fy:0.500876, stop:0." + QString::number(intensity) + " rgba(243, 132, 26, 255), stop:1 rgba(255, 255, 255, 0));"
		"}"
		"QPushButton:pressed"
		"\{"
		"background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.55, fx:0.502, fy:0.500876, stop:0.7 rgba(243, 132, 26, 255), stop:1 rgba(255, 255, 255, 0));"
		"}";


	return style;
}

QString Style::get_h_scrollbar_style(){

	QString arrow_left_filename = Helper::getIconPath() + "/arrow_left.png";
	QString arrow_right_filename = Helper::getIconPath() + "/arrow_right.png";

	QString scrollbar_style = QString("QScrollBar::horizontal \{") +
			      "border: 0px solid " + get_player_back_color()  + "; " +
			      "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, "  +
				  "stop:0 #2F2F2F, stop: 0.2 #323232, " +
				  "stop: 0.8 #323232, stop:1 #2F2F2F); " +
			      "height: 20px;" +
			      "margin: 0 22px 0 22px;" +
			      "border-top: 5px solid " + get_player_back_color()  + "; " +
			  "}" +

			  "QScrollBar::handle:horizontal \{" +
			  "    background: #404040;" +
			  "    min-width: 20px;" +
			  "	   border: 2px solid #2C2C2C; " +
			  "    border-radius: 4px; " +
			  "}" +

			  "QScrollBar::add-line:horizontal \{" +
			  "    border: 1px solid " + get_player_back_color()  + "; " +
			  "    background: " + get_player_back_color()  + "; " +
			  "    width: 20px;" +
			  "    subcontrol-position: right; " +
			  "    subcontrol-origin: margin; " +
			  "    padding: 2px; " +
			  "    border-top: 5px solid " + get_player_back_color()  + "; " +
			  "    image: url(" + arrow_right_filename + "); " +
			  "}" +

			  "QScrollBar::sub-line:horizontal \{" +
			  "    border: 1px solid " + get_player_back_color()  + "; " +
			  "    background: " + get_player_back_color()  + "; " +
			  "    width: 20px;" +
			  "	   subcontrol-position: left;" +
			  "    subcontrol-origin: margin; " +
			  "    padding: 2px; " +
			  "    border-top: 5px solid " + get_player_back_color()  + "; " +
			  "    image: url(" + arrow_left_filename + "); " +

			  "}" +

			  "QScrollBar::left-arrow:horizontal, QScrollBar::right-arrow:horizontal \{" +
			  "    border: 0px solid #3C3C3C; " +
			  "    width: 3px;" +
			  "    height: 0px;" +
			  "}" +



			  "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal \{" +
			  "    background: none;" +
			  "}";

	return scrollbar_style;
}

QString Style::get_menubar_style(bool dark){
	if(!dark) return "";

	QString menubar_style = QString("") +
			"QMenuBar \{" +
			"   color: " + get_player_fore_color() + "; " +
			"	background-color: " + get_player_back_color() + "; " +
			"} " +

			"QMenuBar::item \{" +
			"   color: " + get_player_fore_color() + "; " +
			"   background-color: " + get_player_back_color() + "; " +
			"} " +
			"QMenuBar::item:selected \{ " +
			"   background: #686868; " +
			"}";


	return menubar_style;


}


QString Style::get_menu_style(bool dark){
	if (!dark ) return "";
	QString qmenustyle = QString("") +
		"QMenu { " +
		"     background-color: " + get_player_back_color() + "; " +
		"} " +

		"QMenu::item { " +

		"    background-color: transparent; " +
		"} " +

		"QMenu::item:selected { " +
		"   background: #686868; " +
		"} ";

	return qmenustyle;
}


QString Style::get_sayonara_orange_str(){
return QString("#e8841a");
}
