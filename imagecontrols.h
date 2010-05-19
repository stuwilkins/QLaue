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
 $HeadURL: https://qlaue.svn.sourceforge.net/svnroot/qlaue/trunk/rotatewidget.h $
 
 */

#ifndef _IMAGECONTROLS_H
#define _IMAGECONTROLS_H 1

#include "ui_imagecontrols.h"

class ImageControlWidget : public QWidget
{
	Q_OBJECT
	
public:
	ImageControlWidget(QWidget *parent = 0);
private:
	Ui::ImageControlWidget ui;

public slots:
	void changed(void);
	void setDefaults(void);
	void setScrollbarsEnabled(bool yesno);
	void setImageInfo(QString text);
signals:
	void valueChanged(double, double, double, double);	
	void valueChanged(void);
};

#endif