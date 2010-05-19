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
 
 $Revision: 21 $
 $Author: stuwilkins $
 $Date: 2010-05-06 18:43:12 -0400 (Thu, 06 May 2010) $
 $HeadURL: https://qlaue.svn.sourceforge.net/svnroot/qlaue/trunk/rotatewidget.cpp $
 
 */

#include <QtCore>
#include <QtGui>
#include "imagecontrols.h"

ImageControlWidget::ImageControlWidget(QWidget *parent) : QWidget(parent) 
{
	ui.setupUi(this);
	setFixedSize(QSize(ui.groupBox->width() + 20,ui.groupBox->height() + 20));
	
	setDefaults();
	
	// Setup the connections to the sliders
	connect(ui.ContrastScroll, SIGNAL(valueChanged(int)), this, SLOT(changed(void)));
	connect(ui.BrightnessScroll, SIGNAL(valueChanged(int)), this, SLOT(changed(void)));
	connect(ui.MinimumScroll, SIGNAL(valueChanged(int)), this, SLOT(changed(void)));
	connect(ui.MaximumScroll, SIGNAL(valueChanged(int)), this, SLOT(changed(void)));
}

void ImageControlWidget::setImageInfo(QString text){
	ui.ImageInfo->setText(text);
}

void ImageControlWidget::setScrollbarsEnabled(bool yesno){
	ui.MinimumScroll->setEnabled(yesno);
	ui.MaximumScroll->setEnabled(yesno);
	ui.BrightnessScroll->setEnabled(yesno);
	ui.ContrastScroll->setEnabled(yesno);
}

void ImageControlWidget::setDefaults(void){
	ui.MinimumScroll->setValue(ui.MinimumScroll->minimum());
	ui.MaximumScroll->setValue(ui.MaximumScroll->maximum());
	ui.ContrastScroll->setValue(100);
	ui.BrightnessScroll->setValue(0);
}

void ImageControlWidget::changed(void){
	// Read the values for image
	int contrast = ui.ContrastScroll->value();
	int brightness = ui.BrightnessScroll->value();
	int min = ui.MinimumScroll->value();
	int max = ui.MaximumScroll->value();
	emit valueChanged(brightness, contrast, max, min);
}

