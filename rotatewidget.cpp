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
	
	connect(ui.xaxis, SIGNAL(editingFinished()), this, SLOT(sendNewXYZ()));
	connect(ui.yaxis, SIGNAL(editingFinished()), this, SLOT(sendNewXYZ()));
	connect(ui.zaxis, SIGNAL(editingFinished()), this, SLOT(sendNewXYZ()));
	
	connect(ui.FreeRotateCheckBox, SIGNAL(toggled(bool)), this, SLOT(setFreeRotate(bool)));
	connect(ui.SetGoniometerPushButton, SIGNAL(clicked()), this, SLOT(setGoniometer()));
											   
	free_rotate = false;
}

void RotateWidget::updateRotations(double x, double y, double z){
	ui.xaxis->setText(QString("%1").arg(x * 180.0 / M_PI, 6, 'f', 2));
	ui.yaxis->setText(QString("%1").arg(y * 180.0 / M_PI, 6, 'f', 2));
	ui.zaxis->setText(QString("%1").arg(z * 180.0 / M_PI, 6, 'f', 2));
}

void RotateWidget::xLeftClicked(){
	double newsize = ui.xaxis->text().toDouble() - ui.stepsize->value();
	if(!free_rotate){
		ui.xaxis->setText(QString("%1").arg(newsize));
		sendNewXYZ();
	} else {
		emit valueChanged(-1.0 * ui.stepsize->value() * M_PI / 180, 0, 0);
	}
}

void RotateWidget::xRightClicked(){
	double newsize = ui.stepsize->value() + ui.xaxis->text().toDouble();
	if(!free_rotate){
		ui.xaxis->setText(QString("%1").arg(newsize));
		sendNewXYZ();
	} else {
		emit valueChanged();
		emit valueChanged(ui.stepsize->value() * M_PI / 180, 0, 0);
	}
}

void RotateWidget::yLeftClicked(){
	double newsize = ui.yaxis->text().toDouble() - ui.stepsize->value();
	if(!free_rotate){
		ui.yaxis->setText(QString("%1").arg(newsize));
		sendNewXYZ();
	} else {
		emit valueChanged();
		emit valueChanged(0, -1.0 *ui.stepsize->value() * M_PI / 180, 0);
	}
}

void RotateWidget::yRightClicked(){
	double newsize = ui.stepsize->value() + ui.yaxis->text().toDouble();
	if(!free_rotate){
		ui.yaxis->setText(QString("%1").arg(newsize));
		sendNewXYZ();
	} else {
		emit valueChanged();
		emit valueChanged(0, ui.stepsize->value() * M_PI / 180, 0);
	}	
}

void RotateWidget::zLeftClicked(){
	double newsize = ui.zaxis->text().toDouble() - ui.stepsize->value();
	if(!free_rotate){
		ui.zaxis->setText(QString("%1").arg(newsize));
		sendNewXYZ();
	} else {
		emit valueChanged();
		emit valueChanged(0, 0, -1.0 *ui.stepsize->value() * M_PI / 180);
	}	
}

void RotateWidget::zRightClicked(){
	double newsize = ui.stepsize->value() + ui.zaxis->text().toDouble();
	if(!free_rotate){
		ui.zaxis->setText(QString("%1").arg(newsize));
		sendNewXYZ();
	} else {
		emit valueChanged();
		emit valueChanged(0, 0, ui.stepsize->value() * M_PI / 180);
	}	
}

void RotateWidget::sendNewXYZ(void){
	emit valueChanged(ui.xaxis->text().toDouble() * M_PI / 180,
					  ui.yaxis->text().toDouble() * M_PI / 180,
					  ui.zaxis->text().toDouble() * M_PI / 180);
	emit valueChanged();
}

void RotateWidget::setFreeRotate(bool newState){
	free_rotate = newState;
	ui.FreeRotateCheckBox->setChecked(newState);
	if(newState){
		ui.zaxis->setEnabled(false);
		ui.yaxis->setEnabled(false);
		ui.xaxis->setEnabled(false);
		ui.xaxis->setText("");
		ui.yaxis->setText("");
		ui.zaxis->setText("");
	} else {
		ui.zaxis->setEnabled(true);
		ui.yaxis->setEnabled(true);
		ui.xaxis->setEnabled(true);
		ui.xaxis->setText("0.00");
		ui.yaxis->setText("0.00");
		ui.zaxis->setText("0.00");
	}
	
	emit freeRotate(newState);
}

bool RotateWidget::freeRotate(void){
	return free_rotate;
}

void RotateWidget::setGoniometer(void){
	ui.xaxis->setText("0.0");
	ui.yaxis->setText("0.0");
	ui.zaxis->setText("0.0");
	emit setGoniometerPos();
}