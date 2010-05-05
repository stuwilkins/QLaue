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
	
	$Revision$
	$Author$
	$Date$
	$HeadURL$

*/

#ifndef _SETUBMAT_H
#define _SETUBMAT_H 1

#include "matrix.h"
#include "ui_setubmat.h"

class UMatrixDialog : public QDialog
{
	Q_OBJECT
	
	Matrix primary;
	Matrix secondary;
public:
	UMatrixDialog(QWidget *parent = 0);
	Matrix getPrimary(void) { return primary; }
	Matrix getSecondary(void) { return secondary; }
private slots:
	void calcClicked(void);	
private:
	Ui::UMatrixDialog ui;
};

#endif