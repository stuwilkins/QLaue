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

#include <QtGui>
#include "ui_setubmat.h"
#include "setubmat.h"
#include "version.h"

UMatrixDialog::UMatrixDialog(QWidget *parent) : 
QDialog(parent), primary(3,1), secondary(3,1) {
	ui.setupUi(this);
	connect(ui.buttonBox,SIGNAL(accepted()), this, SLOT(calcClicked()));
}

void UMatrixDialog::calcClicked(void){
	primary.Set(0,0,ui.PH->text().toDouble());
	primary.Set(1,0,ui.PK->text().toDouble());
	primary.Set(2,0,ui.PL->text().toDouble());
	secondary.Set(0,0,ui.SH->text().toDouble());
	secondary.Set(1,0,ui.SK->text().toDouble());
	secondary.Set(2,0,ui.SL->text().toDouble());
	
	// Check that the vectors are non-zero
	
	if(primary.mag() == 0){
		QMessageBox::critical(	this, tr(APP_NAME),
								tr("Primary vector is equal to zero.\n"),
								QMessageBox::Cancel);
		return;
	}
	
	if(secondary.mag() == 0){
		QMessageBox::critical(	this, tr(APP_NAME),
								tr("Secondary vector is equal to zero.\n"),
								QMessageBox::Cancel);
		return;
	}
	
	// Now check if they are paralell
	
	if(primary.dot(secondary) == (primary.mag() * secondary.mag())){
		QMessageBox::critical(	this, tr(APP_NAME),
								tr("Primary and Secondary vectors are paralell.\n"),
								QMessageBox::Cancel);
		return;
	}
	
	accept();
}

