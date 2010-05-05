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
#include "lauewidget.h"
#include "lauecontrols.h"

LaueControls::LaueControls(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);	
	setFixedSize(QSize(ui.groupBox->width() + 20,ui.groupBox->height() + 20));
	
	// Make connections
	
	connect(ui.ShowSimulationIntensities, SIGNAL(toggled(bool)), ui.DetLevelSpinBox, SLOT(setEnabled(bool)));
	connect(ui.ShowSimulationIntensities, SIGNAL(toggled(bool)), ui.SatLevelSpinBox, SLOT(setEnabled(bool)));
	connect(ui.ShowSimulationIntensities, SIGNAL(toggled(bool)), ui.DetLevelSlider, SLOT(setEnabled(bool)));
	connect(ui.ShowSimulationIntensities, SIGNAL(toggled(bool)), ui.SatLevelSlider, SLOT(setEnabled(bool)));
	connect(ui.ShowSimulationIntensities, SIGNAL(toggled(bool)), ui.ShowCharacteristicCheckBox, SLOT(setEnabled(bool)));
	connect(ui.ShowSimulationIntensities, SIGNAL(toggled(bool)), ui.IntensityFilmLikeCheckBox, SLOT(setEnabled(bool)));
	
	connect(ui.DetLevelSpinBox, SIGNAL(valueChanged (int)), ui.DetLevelSlider, SLOT(setValue(int)));
	connect(ui.SatLevelSpinBox, SIGNAL(valueChanged (int)), ui.SatLevelSlider, SLOT(setValue(int))); 
	connect(ui.DetLevelSlider, SIGNAL(valueChanged (int)), ui.DetLevelSpinBox, SLOT(setValue(int))); 
	connect(ui.SatLevelSlider, SIGNAL(valueChanged (int)), ui.SatLevelSpinBox, SLOT(setValue(int)));
	
	connect(ui.SpotSizeSlider, SIGNAL(valueChanged (int)), ui.SpotSizeSpinBox, SLOT(setValue(int))); 
	connect(ui.SpotSizeSpinBox, SIGNAL(valueChanged (int)), ui.SpotSizeSlider, SLOT(setValue(int)));
	
	connect(ui.AccuracySlider, SIGNAL(valueChanged (int)), ui.AccuracySpinBox, SLOT(setValue(int))); 
	connect(ui.AccuracySpinBox, SIGNAL(valueChanged (int)), ui.AccuracySlider, SLOT(setValue(int)));
	
	connect(ui.sampleFilmD, SIGNAL(editingFinished()), this, SLOT(dChanged()));
	connect(ui.SpotSizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(spotChanged(int)));
	connect(ui.SatLevelSpinBox, SIGNAL(valueChanged(int)), this, SLOT(satChanged(int)));
	connect(ui.DetLevelSpinBox, SIGNAL(valueChanged(int)), this, SLOT(detChanged(int)));
	connect(ui.ShowSimulationCheckbox, SIGNAL(stateChanged(int)), this, SLOT(simChanged(int)));
	connect(ui.ShowSimulationIntensities, SIGNAL(stateChanged(int)), this, SLOT(intChanged(int)));
	connect(ui.ShowCharacteristicCheckBox, SIGNAL(stateChanged(int)), this, SLOT(charIntChanged(int)));
	connect(ui.AccuracySpinBox, SIGNAL(valueChanged(int)), this, SLOT(accuracyChanged(int)));
	connect(ui.IntensityFilmLikeCheckBox, SIGNAL(toggled(bool)), this, SLOT(intensityFilmLikeChanged(bool)));
}

void LaueControls::intensityFilmLikeChanged(bool newval){
	emit showIntensitiesAsColor(newval);
}

void LaueControls::charIntChanged(int newval){
	emit showCharacteristicChanged(newval);
}

void LaueControls::dChanged(void){
	emit sampleFilmDChanged(ui.sampleFilmD->value());
}

void LaueControls::dValueChanged(double newval){
	ui.sampleFilmD->setValue(newval);
}

void LaueControls::spotChanged(int newval){
	emit spotSizeChanged(newval);
	ui.SpotSizeSpinBox->setValue(newval);
}

void LaueControls::accuracyChanged(int newval){
	emit accuracyLevelChanged(newval);
	ui.AccuracySpinBox->setValue(newval);
}

void LaueControls::detChanged(int newval){
	emit  detLevelChanged(newval);
	ui.DetLevelSpinBox->setValue(newval);
}

void LaueControls::satChanged(int newval){
	emit  satLevelChanged(newval);
	ui.SatLevelSpinBox->setValue(newval);
}

void LaueControls::intChanged(int newval){
	emit showSimulationIntensities(newval);
	//ui.ShowSimulationIntensities->setChecked(newval);
}

void LaueControls::simChanged(int newval){
	emit showSimulation(newval);
	//ui.ShowSimulationCheckbox->setChecked(newval);
}

void LaueControls::setAll(LaueFilm *film){
	
	ui.SpotSizeSpinBox->setValue(film->getSpotSize());
	ui.sampleFilmD->setValue(film->getLaue()->getDistance());
	ui.SatLevelSpinBox->setValue((int)(film->getLaue()->getDetSatLevel() * 100));
	ui.DetLevelSpinBox->setValue((int)(film->getLaue()->getDetLevel() * 100));
	ui.AccuracySpinBox->setValue(film->getAccuracy());
	ui.ShowSimulationCheckbox->setChecked(film->getDisplayLaue());
	ui.ShowSimulationIntensities->setChecked(film->getDisplayIntensities());
	ui.ShowCharacteristicCheckBox->setChecked(film->getDisplayCharacteristicInt());
	ui.IntensityFilmLikeCheckBox->setChecked(film->getDisplayIntensitiesAsColor());

}