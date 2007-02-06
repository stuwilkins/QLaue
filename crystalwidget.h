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

#ifndef _CRYSTALWIDGET_H
#define _CRYSTALWIDGET_H

#include "ui_crystalwidget.h"
#include "matrix.h"

class CrystalWidget : public QWidget {
	Q_OBJECT
public:
	CrystalWidget(QWidget *parent = 0);
public slots:
	void displayOrientation(Matrix axis1, Matrix axis2, Matrix axis3);	
private:
	Ui::CrystalWidget ui;
};

#endif