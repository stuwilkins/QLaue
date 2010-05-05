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

#include "filmsizedialog.h"

FilmSizeDialog::FilmSizeDialog(QWidget* parent) :
QDialog(parent)
{
	ui.setupUi(this);
	width[0] = 89.0; height[0] = 111.25;
	filmTypes << "Polaroid 57 (4\" x 5\")" << "Other";
	ui.FilmTypeComboBox->insertItems(0,filmTypes);
	connect(ui.FilmTypeComboBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(boxChanged(int)));
	
	QSettings settings;
	settings.beginGroup("FilmSize");
	
	int newindex = 1; // Set to custom index
	
	if(settings.value("Custom").toBool() == false){
		newindex = settings.value("StandardFilmSize", 0).toInt();
	} else {
		ui.FilmWidth->setText(QString("%1").arg(settings.value("CustomWidth",100).toDouble()));
		ui.FilmHeight->setText(QString("%1").arg(settings.value("CustomHeight",100).toDouble()));
	}
		
	ui.FilmTypeComboBox->setCurrentIndex(newindex);
	boxChanged(newindex);
}

void FilmSizeDialog::boxChanged(int newindex){
	if(ui.FilmTypeComboBox->currentText() == "Other"){
		ui.FilmWidth->setEnabled(true);
		ui.FilmHeight->setEnabled(true);
	} else {
		ui.FilmWidth->setEnabled(false);
		ui.FilmHeight->setEnabled(false);
		
		ui.FilmWidth->setText(QString("%1").arg(width[newindex]));
		ui.FilmHeight->setText(QString("%1").arg(height[newindex]));
	}
}

void FilmSizeDialog::accept(void){
	qDebug("FilmSizeDialog::closeEvent()");
	if(ui.DefaultCheckBox->isChecked()){
		QSettings settings;
		settings.beginGroup("FilmSize");
		if(ui.FilmTypeComboBox->currentText() == "Other"){
			settings.setValue("Custom",(bool)true);
			settings.setValue("CustomWidth",ui.FilmWidth->text().toDouble());
			settings.setValue("CustomHeight",ui.FilmHeight->text().toDouble());
		} else {
			settings.setValue("Custom",(bool)false);
			settings.setValue("StandardFilmSize",ui.FilmTypeComboBox->currentIndex());
		}
	}
	done(QDialog::Accepted);
}