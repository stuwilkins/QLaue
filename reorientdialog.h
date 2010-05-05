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

#ifndef _REORIENTDIALOG_H
#define _REORIENTDIALOG_H 1

#include "ui_reorientdialog.h"
#include "laue.h"
#include "crystal.h"

class ReorientDialog : public QDialog
{
	Q_OBJECT
	
public:
	ReorientDialog(QWidget *parent = 0);
	void setLaue(lauegram *newLaue);
	void setCrystal(Crystal *newCrystal);
	bool getSelectedOrientation(void);
	void getAngles(double *x, double *y, double *z);
	
	// Return values
	
	enum {
		CancelClicked = 0,
		OKClicked = 1,
		PrintClicked = 2
	};
	
private:
	Ui::ReorientDialog ui;
	lauegram* laue;
	Crystal* crystal;
	bool selectedOrientation;
	double angleX, angleY, angleZ;
	
	void setupTable(void);
	void setTableItem(QTableWidget *table, int x, int y, double val);
	void setBlankTableItem(QTableWidget *table, int x, int y);
	
private slots:
	void go(void);
	void okPressed(void);
	void printPressed(void);
	void cancelPressed(void);
};

#endif