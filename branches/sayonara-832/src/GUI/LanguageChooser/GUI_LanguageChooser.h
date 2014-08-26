/* GUI_LanguageChooser.h */

/* Copyright (C) 2011-2014  Lucio Carreras
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



#ifndef GUI_LANGUAGECHOOSER_H
#define GUI_LANGUAGECHOOSER_H

#include <QDialog>
#include <QMap>
#include <QShowEvent>

#include "GUI/ui_GUI_LanguageChooser.h"

class GUI_LanguageChooser : public QDialog, private Ui::GUI_LanguageChooser
{
    Q_OBJECT

public:

    explicit GUI_LanguageChooser(QWidget *parent = 0);
    virtual ~GUI_LanguageChooser();

signals:
    void sig_language_changed(QString);
    
public slots:
    void language_changed(bool b);

private slots:
    void combo_changed(int);
    void ok_clicked();


private:

    QMap<QString, QString> _map;
    int _last_idx;
    void renew_combo();

protected:
    void showEvent(QShowEvent *);
    
};

#endif // GUI_LANGUAGECHOOSER_H
