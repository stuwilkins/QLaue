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

#include <QtCore>
#include <QtGui>
#include "rotatewidget.h"
#include "matrix.h"

RotateWidget::RotateWidget(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	setFixedSize(QSize(ui.groupBox->width() + 20,ui.groupBox->height() + 20));
	
	connect(ui.xleft, SIGNAL(clicked()), this, SLOT(xLeftClicked()));
	connect(ui.xright, SIGNAL(clicked()), this, SLOT(xRightClicked()));
	connect(ui.yleft, SIGNAL(clicked()), this, SLOT(yLeftClicked()));
	connect(ui.yright, SIGNAL(clicked()), this, SLOT(yRightClicked()));
	connect(ui.zleft, SIGNAL(clicked()), this, SLOT(zLeftClicked()));
	connect(ui.zright, SIGNAL(clicked()), this, SLOT(zRightClicked()));
}

void RotateWidget::xLeftClicked(){
	emit valueChanged(-1.0 * ui.stepsize->value() * M_PI / 180, 0, 0);
}

void RotateWidget::xRightClicked(){
	emit valueChanged();	
	emit valueChanged(ui.stepsize->value() * M_PI / 180, 0, 0);
}

void RotateWidget::yLeftClicked(){
	emit valueChanged();
	emit valueChanged(0, -1.0 *ui.stepsize->value() * M_PI / 180, 0);
}

void RotateWidget::yRightClicked(){
	emit valueChanged();
	emit valueChanged(0, ui.stepsize->value() * M_PI / 180, 0);	
}

void RotateWidget::zLeftClicked(){
	emit valueChanged();
	emit valueChanged(0, 0, -1.0 *ui.stepsize->value() * M_PI / 180);
}

void RotateWidget::zRightClicked(){
	emit valueChanged();
	emit valueChanged(0, 0, ui.stepsize->value() * M_PI / 180);
}

