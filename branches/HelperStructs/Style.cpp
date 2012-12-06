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

#include <sstream>

#define NEWLINE "\n";


static QString get_fg_color(int val_bg, bool with_str=true){

    QString ret_str = "";
    if(with_str) ret_str = QString(" color: ");
    if(val_bg > 96)
        return  ret_str + QString("#202020; ");

    else
        return ret_str + QString("#D8D8D8; ");
}

QString Style::get_player_back_color(){
	return QString("#383838");
}

QString Style::get_player_fore_color(){
	return QString("#D8D8D8");
}

QString Style::get_tv_style(bool dark, QPalette* p){

    if(!dark) return "";

    QString style;


    int highlight_val;
    if(p){
        QColor col_highlight = p->color(QPalette::Active, QPalette::Highlight);
        highlight_val = col_highlight.lightness();
    }

    else highlight_val = 255;

    QString fg_color = get_fg_color(highlight_val, false);
    if(p){
        p->setColor(QPalette::Active, QPalette::HighlightedText, QColor(0, 0, 0));
    }

    style = QString("border: 1px solid #282828; "
                    "background-color: #2C2C2C;  "
                    "alternate-background-color: #242424; "
                    /*"color: #D8D8D8;"*/);

    return  style;

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

QString Style::get_pushbutton_style(bool dark){

    if(!dark) return "";


    QString dark_grey = "#383838";
    QString darker_grey = "#242424";
    QString lighter_grey = "#424242";
    QString lighter_grey2 = "#525252";
    QString orange = "#e8841a";

    QString style = QString("QPushButton {") +
            "   border-left: 1px solid " + lighter_grey2 + ";" +
            "   border-bottom: 1px solid " + lighter_grey2 + ";" +
            "   border-right: 1px solid " + lighter_grey2 + ";" +
            "   border-top: 1px solid " + lighter_grey2 + ";" +
            "   border-radius: 6px; " +
            "   background: " + lighter_grey + ";" +
            "   padding: 4px;"
            "}" +

    "QPushButton:pressed {" +
                "   border-left: 1px solid " + orange + ";" +
                "   border-bottom: 1px solid " + orange + ";" +
                "   border-right: 1px solid " + orange + ";" +
                "   border-top: 1px solid " + orange + ";" +
                "   border-radius: 6px; " +
                "   padding: 4px;" +
                "   background: " + dark_grey + ";" +
                "}" +

                "QPushButton:disabled {" +
                "   border-left: 1px solid " + lighter_grey + ";" +
                "   border-bottom: 1px solid " + lighter_grey + ";" +
                "   border-right: 1px solid " + lighter_grey + ";" +
                "   border-top: 1px solid " + lighter_grey + ";" +
                 "color: " + lighter_grey + ";" +
                 "background: " + dark_grey + ";" +
                "}";





    return style;
}

QString Style::get_tabwidget_style(bool dark){
    if(!dark ) return "";


    QString dark_grey = "#383838";
    QString darker_grey = "#242424";
    QString lighter_grey = "#424242";
    QString lighter_grey2 = "#525252";
    QString orange = "#e8841a";
   QString style = QString("QTabWidget::pane { ") +
           "     border: 1px solid " + lighter_grey2 + ";" +
    "}" +

    "QTabWidget::tab-bar {" +
    "    left: 5px; /* move to the right by 5px */" +
    "}" +


    "QTabBar::tab {" +
    "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1," +
           "                        stop: 0 " + lighter_grey2 + ", stop: 0.4 " + dark_grey + "," +
           "                                stop: 0.5 " + dark_grey + ", stop: 1.0 " + lighter_grey + ");" +
           "    border: 1px solid " + darker_grey + ";" +
   // "    border-bottom-color: #C2C7CB; " +
    "    border-top-left-radius: 4px;" +
    "    border-top-right-radius: 4px;" +
    "    min-width: 8ex;" +
    "    padding: 4px;" +
    "}" +

   "QTabBar::tab:disabled {" +
          "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1," +
          "                        stop: 0 " + lighter_grey2 + ", stop: 0.4 " + dark_grey + "," +
          "                                stop: 0.5 " + dark_grey + ", stop: 1.0 " + lighter_grey + ");" +
          "    border: 1px solid " + darker_grey + ";" +
           "    color: " + lighter_grey2 + ";" +
   "}" +

    "QTabBar::tab:selected, QTabBar::tab:hover {" +
           "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1," +
                  "                        stop: 0 " + lighter_grey2 + ", stop: 0.4 " + lighter_grey + "," +
                  "                                stop: 0.5 " + lighter_grey + ", stop: 1.0 " + lighter_grey2 + ");" +
                  "    border: 1px solid " + darker_grey + ";" +
    "}" +



    "QTabBar::tab:selected {" +
    "    border-color: " + darker_grey + ";" +
    "    border-bottom-color: " + lighter_grey2 + ";" +
    "}" +

    "QTabBar::tab:!selected {" +
    "    margin-top: 2px; /* make non-selected tabs look smaller */" +
    "}";

   return style;

}

QString Style::get_lineedit_style(bool dark){

    QString dark_grey = "#383838";
    QString grey = "#424242";
    QString lighter_grey = "#525252";
    QString orange = "#e8841a";

    if(!dark) return "";

    QString style = QString("QLineEdit {") +
            "    border: 1px solid #2B2B2B;" +
            "    border-radius: 4px; " +
            "    background: " + lighter_grey + ";" +
            "}"+

            "QLineEdit::focus {"+
            "    border-radius: 4px; " +
            "    border: 1px solid " + orange + ";" +
            "}"

            "QLineEdit:read-only {" +
            "    border-radius: 4px; " +
            "    border: 1px solid #2B2B2B;" +
            //"    background: " + grey + ";" +
            "}" +


             "QLineEdit:disabled {" +
            "    border-radius: 4px; " +
            "    border: 1px solid #2B2B2B;" +
            "    background: " + dark_grey + ";" +
            "    color: " + lighter_grey + "; " +
            "}";

    return style;


}

QString Style::get_combobox_style(bool dark){

    QString dark_grey = "#383838";
    QString lighter_grey = "#525252";
    QString orange = "#e8841a";

    if(!dark) return "";

    else{
        QString arrow_down_filename = Helper::getIconPath() + "/arrow_down.png";

        QString style =
        QString("QComboBox {") +
        "     border: 1px solid #2B2B2B;" +
        "     border-radius: 3px;"+
        "     padding: 1px 18px 1px 3px;"+
        "     min-width: 6em;"+

        " }"+

         "QComboBox:editable {"+
                "     background: " + lighter_grey + ";" +

         "}"+

         "QComboBox:!editable, QComboBox::drop-down:editable {"+
         "     background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "+
         "                                 stop: 0 #555555, stop: 0.4 " + dark_grey + ");" +
         "}"+

                "QComboBox:!editable:on, QComboBox::drop-down:editable:on {"+
         "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"+
         "                                stop: 0 #f1b42a, stop: 0.4 #e8841a,"+
         "                                stop: 0.5 #e8841a, stop: 1.0 #e8841a);"+
         "}"+

                "QComboBox:!editable:off, QComboBox::drop-down:editable:off {"+
         "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"+
         "                                stop: 0 #f1b42a, stop: 0.4 #e8841a,"+
         "                                stop: 0.5 #e8841a, stop: 1.0 #e8841a);"+
         "}"+

         "QComboBox:on { /* shift the text when the popup opens */"+
         "    padding-top: 3px;"+
         "    padding-left: 4px;"+
         "}"+

         "QComboBox::drop-down {"+
         "    width: 18px;"+

         "    border-left-width: 1px;"+
         "    border-left-color: #242424;"+
         "    border-left-style: solid; /* just a single line */"+
         "    border-top-right-radius: 3px; /* same radius as the QComboBox */"+
         "    border-bottom-right-radius: 3px;"+
         "}"+

         "QComboBox::down-arrow {"+
                "   height: 12px; " +
                "   width: 10px; " +
                " padding: 2px; " +
                "    image: url("+ arrow_down_filename +");"+
         "}"

         "QComboBox:focus {"

            "    border-radius: 4px; " +
            "    border: 1px solid " + orange + ";" +
         "}"

         "";



        return style;
    }

    return "";

}


QString Style::get_v_scrollbar_style(){

	QString arrow_down_filename = Helper::getIconPath() + "/arrow_down.png";
	QString arrow_up_filename = Helper::getIconPath() + "/arrow_up.png";
    QString dark_grey = "#404040";

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


QString Style::get_btn_style(bool dark, int intensity){

    QString style =
        QString("QPushButton") +
		"{"
        "    color: white; "
        "    border-radius: 7; "
        "    padding: 1px; "
        "    padding-left: 1px; "
        "    padding-right: 1px; "
		"}"

        "QPushButton:checked"
		"\{"
        "    background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.55, fx:0.502, fy:0.500876, stop:0." + QString::number(intensity) + " rgba(243, 132, 26, 255), stop:1 rgba(255, 255, 255, 0));"
		"}"

        "QPushButton:pressed"
		"\{"
        "    background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.55, fx:0.502, fy:0.500876, stop:0.7 rgba(243, 132, 26, 255), stop:1 rgba(255, 255, 255, 0));"
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


QString Style::get_spinbox_style(bool dark){

    if(!dark) return "";

    QString dark_grey = "#383838";
    QString darker_grey = "#2B2B2B";
    QString lighter_grey2 = "#525252";
    QString orange = "#e8841a";

    QString arrow_down_filename = Helper::getIconPath() + "/arrow_down.png";
    QString arrow_up_filename = Helper::getIconPath() + "/arrow_up.png";
    QString style = QString("") +

    "QSpinBox {" +
    "    background: " + lighter_grey2 + "; " +
    "    border: 1px solid " + darker_grey + "; " +
    "}" +

    "QSpinBox:focus {" +
    "    padding-right: 15px;" +
    "    background: " + lighter_grey2 + "; " +
    "    border: 1px solid " + orange + "; " +
    "}" +


    "QSpinBox::up-button {" +
    "     subcontrol-origin: margin;" +
    "     subcontrol-position: top right;" +
    "     width: 12px;" +
    "     margin-top: 1px; "
    "     margin-right: 1px; "
    "     padding: 2px;" +
    "     "
    "     background: " + dark_grey + "; " +
    "}" +


    "QSpinBox::up-button:hover {"+
    "    background: " + lighter_grey2  + "; "+
    "}"+




    "QSpinBox::up-arrow {" +
 "    image: url(" + arrow_up_filename + ");" +
    "     width: 7px;" +
    "     height: 7px;" +
    " }" +

    "QSpinBox::down-button {"
    "     subcontrol-origin: border;"
    "     subcontrol-position: bottom right;"
    "     padding: 2px;"
    "     margin-bottom: 1px; "
    "     margin-right: 1px; "
    "     width: 12px;"
    "     background: " + dark_grey + "; "
    " }" +



    "QSpinBox::down-button:hover {"+
    "    background: " + lighter_grey2  + "; "+
    "}"+

    "QSpinBox::down-arrow {" +
    "    image: url(" + arrow_down_filename + ");" +
    "    width: 7px;" +
    "    height: 7px;" +
    "}";

    return style;
}


QString Style::get_v_slider_style(bool dark, int percent){
    if(!dark) return "";

    QString darker_grey = "#2B2B2B";
    QString dark_grey = "#525252";



    QString orange = "#e8841a";
    QString back_col = orange;

    if(percent > 0)
    {
        double p = percent * 1.0 / 100.0;
        int r, g, b;
        b = 0;
        if(p < 0.6) {
            r=255;
        }

        else {
            r = (-255.0 / 0.6) * (p-0.6) + 255;
        }

        g = (255.0) * p;


        QString s_r;
        QString s_g;
        QString s_b;

        s_r.sprintf("%02X", r);
        s_g.sprintf("%02X", g);
        s_b.sprintf("%02X", b);


        back_col = "#" + s_r + s_g + s_b;
    }


    QString style =  QString("QSlider {") +
            "border-radius: 4px; " +
            "background: #383838; " +

        "}" +

        "QSlider::handle:vertical {" +
        //"    background: #525252;" +
       "     background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "+
         "                                 stop: 0 #6A6A6A, stop: 0.4 " + dark_grey + ");" +
        "    height: 10px;" +
        "    margin-left: -5px; " +
        "    margin-right: -5px; " +
        "    min-width: 12px;" +
        "	 border: 1px solid #2C2C2C; " +
        "    border-radius: 4px; " +
        "}" +

        "QSlider::handle:vertical:disabled {" +
        "    background: transparent;"
        "    height: 0px;"
        "	 border-width: 0px;"
        "}" +


        "QSlider::groove:vertical { " +
        "    background: #383838;" +
        "    width: 6px; " +
        "    left: 10px; right: 10px; " +
        "}" +


        "QSlider::add-page:vertical {"+
        /*"    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, "  +
        "          stop:0 #E88417, stop: 0.3 #C46600, " +
        "          stop: 0.7 #C46600, stop:1 #E88417); " +*/
        //"      background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 rgba(255, 0, 0, 255), stop:0.505051 rgba(255, 228, 0, 255), stop:1 rgba(55, 239, 78, 255)); "

        "    background-color: " + back_col + "; "
        "    border-radius: 2px;" +

       "}"+

        "QSlider::sub-page:vertical, QSlider::add-page:vertical:disabled {"+
        "   background: " + darker_grey + ";"+
        //     "    background-color: " + back_col + "; "
        "   border-radius: 2px;" +

        "}";

    return style;
}

QString Style::get_h_slider_style(bool dark){
   if(!dark) return "";

    QString darker_grey = "#2B2B2B";
    QString dark_grey = "#525252";


    QString style =  QString("QSlider {") +
            "border-radius: 4px; " +
            "background: #383838; " +
        "}" +

        "QSlider::handle:horizontal {" +
        //"    background: #525252;" +
        "     background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "+
         "                                 stop: 0 #6A6A6A, stop: 0.4 " + dark_grey + ");" +
        "    width: 10px;" +
        "    margin-top: -5px; " +
        "    margin-bottom: -5px; " +
        "    min-height: 12px;" +
        "	 border: 1px solid #2C2C2C; " +
        "    border-radius: 4px; " +
        "}" +

        "QSlider::handle:horizontal:disabled {" +
        "    background: transparent;"
        "    width: 0px;"
        "	 border-width: 0px;"
        "}" +


        "QSlider::handle:horizontal:disabled {" +
        "    width: 0px;"
        "}" +


        "QSlider::groove:horizontal { "
        "    background: #383838;"
        "    height: 6px; "
        "    top: 10px; bottom: 10px; "
        "}"


        "QSlider::add-page:horizontal {"
        "   background: " + darker_grey + ";"
        "   border-radius: 2px;"
       "}"

        "QSlider::sub-page:horizontal {"
        "   border-radius: 2px;"
        "   background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "          stop:0 #E88417, stop: 0.3 #C46600, "
        "          stop: 0.7 #C46600, stop:1 #E88417); "

        "}";

    return style;
}


QString  Style::get_cb_style(bool dark){
    if(!dark) return "";

    QString darker_grey = "#242424";
    QString lighter_grey = "#424242";
    QString lighter_grey2 = "#525252";

    QString style = QString("QCheckBox {") +
        "    spacing: 5px;"
        "}"

        "QCheckBox::disabled {"
        "    spacing: 5px;"
        "    color: " + lighter_grey + ";"
        "}"

        "QCheckBox::indicator {"
        "    border: 1px solid " + darker_grey + ";"
        "    padding: 2px; "
        "    width: 8px;"
        "    height: 8px;"
        "    max-width: 8px;"
        "    max-height: 8px;"
        "   border-radius: 3px;"

        "    background: " + lighter_grey2 + ";"
        "}"

        "QCheckBox::indicator:disabled {"
        "    border: 1px solid #2B2B2B;"
       "    background: " + lighter_grey + ";"
        "}"



        "QCheckBox::indicator:checked {"
        "    image: url(" + Helper::getIconPath() + "/cb_checked.png" + ");"
        "}"


        "QCheckBox::indicator:checked:disabled {"
        "    image: url(" + Helper::getIconPath() + "/cb_checked_disabled.png" + ");"
        "}"


        "";

        return style;
}


QString  Style::get_rb_style(bool dark){
    if(!dark) return "";


    QString darker_grey = "#242424";
    QString lighter_grey = "#424242";
    QString lighter_grey2 = "#525252";

    QString style = QString("QRadioButton {") +
        "    spacing: 5px;"
        "}"

        "QRadioButton::disabled {"
        "    spacing: 5px;"
        "    color: " + lighter_grey + ";"
        "}"

        "QRadioButton::indicator {"
        "    border: 1px solid " + darker_grey + ";"
        "    padding: 1px; "
        "    width: 8px;"
        "    height: 8px;"
        "    max-width: 8px;"
        "    max-height: 8px;"
        "   border-radius: 5px;"

        "    background: " + lighter_grey2 + ";"
        "}"

        "QRadioButton::indicator:disabled {"
        "    border: 1px solid #2B2B2B;"
       "    background: " + lighter_grey + ";"
        "}"



        "QRadioButton::indicator:checked {"
        "    image: url(" + Helper::getIconPath() + "/rb_checked.png" + ");"
        "}"


        "QRadioButton::indicator:checked:disabled {"
        "    image: url(" + Helper::getIconPath() + "/rb_checked_disabled.png" + ");"
        "}"


        "";

        return style;
}
