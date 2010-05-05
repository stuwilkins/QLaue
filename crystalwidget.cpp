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

#include "matrix.h"
#include "crystalwidget.h"

CrystalWidget::CrystalWidget(QWidget *parent) :
QWidget(parent)
{
	ui.setupUi(this);
	setFixedSize(geometry().size());
}

void CrystalWidget::displayOrientation(Matrix axis1, Matrix axis2, Matrix axis3){
	ui.OrientationLine1_1->setText(QString("%1").arg(axis1.Get(0,0),5,'f',3));
	ui.OrientationLine1_2->setText(QString("%1").arg(axis1.Get(1,0),5,'f',3));
	ui.OrientationLine1_3->setText(QString("%1").arg(axis1.Get(2,0),5,'f',3));
	
	ui.OrientationLine2_1->setText(QString("%1").arg(axis2.Get(0,0),5,'f',3));
	ui.OrientationLine2_2->setText(QString("%1").arg(axis2.Get(1,0),5,'f',3));
	ui.OrientationLine2_3->setText(QString("%1").arg(axis2.Get(2,0),5,'f',3));
	
	ui.OrientationLine3_1->setText(QString("%1").arg(axis3.Get(0,0),5,'f',3));
	ui.OrientationLine3_2->setText(QString("%1").arg(axis3.Get(1,0),5,'f',3));
	ui.OrientationLine3_3->setText(QString("%1").arg(axis3.Get(2,0),5,'f',3));	
}