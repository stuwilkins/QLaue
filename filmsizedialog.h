/*

	QLaue - Orientation of single crystal samples by the Laue method. 
	Copyright (C) 2007 Stuart Brian Wilkins

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
	
	$Revision:$
	$Author:$
	$Date:$
	$HeadURL:$

*/

#ifndef _FILMSIZEDIALOG_H
#define _FILMSIZEDIALOG_H

#include <QtCore>
#include <QtGui>
#include "ui_filmsizedialog.h"

class FilmSizeDialog : public QDialog {
	Q_OBJECT
	double width[1];
	double height[1];
	QStringList filmTypes;
private:
	Ui::FilmSizeDialog ui;
public:
	FilmSizeDialog(QWidget* parent = 0);	
private slots:
	void boxChanged(int newindex);
void accept(void);
public slots:
	double getFilmWidth(void)			{ return ui.FilmWidth->text().toDouble(); }
	double getFilmHeight(void)			{ return ui.FilmHeight->text().toDouble(); }
};

#endif