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

#ifndef _LAUECONTROLS_H
#define _LAUECONTROLS_H

#include "laue.h"
#include "lauewidget.h"
#include "ui_lauecontrols.h"

class LaueControls : public QWidget
{
	Q_OBJECT
public:
	LaueControls(QWidget *parent = 0);
private:
	Ui::LaueControls ui;
public slots:
	void dChanged(void);
	void setAll(LaueFilm *film);
	void dValueChanged(double newval);
	void spotChanged(int newval);
	void satChanged(int newval);
	void detChanged(int newval);
	void simChanged(int newval);
	void intChanged(int newval);
	void accuracyChanged(int newval);
	void charIntChanged(int newval);
	void intensityFilmLikeChanged(bool newval);
signals:
	void sampleFilmDChanged(double d);
	void showSimulation(int yesno);
	void showSimulationIntensities(int yesno);
	void detLevelChanged(int value);
	void satLevelChanged(int value);
	void spotSizeChanged(int value);
	void accuracyLevelChanged(int value);
	void showCharacteristicChanged(int newval);
	void showIntensitiesAsColor(bool yesno);
};

#endif