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
#include "reorientdialog.h"
#include "laue.h"

ReorientDialog::ReorientDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);
	
	ui.okButton->setEnabled(false);
	ui.printButton->setEnabled(false);
	
	selectedOrientation = false;
	
	connect(ui.XGoButton,SIGNAL(clicked()),this, SLOT(go()));
	connect(ui.ZoneButton, SIGNAL(toggled(bool)), ui.ZHLineEdit, SLOT(setEnabled(bool)));
	connect(ui.ZoneButton, SIGNAL(toggled(bool)), ui.ZKLineEdit, SLOT(setEnabled(bool)));
	connect(ui.ZoneButton, SIGNAL(toggled(bool)), ui.ZLLineEdit, SLOT(setEnabled(bool)));
	connect(ui.okButton, SIGNAL(clicked()),this, SLOT(okPressed()));
	connect(ui.printButton, SIGNAL(clicked()),this, SLOT(printPressed()));
	connect(ui.cancelButton, SIGNAL(clicked()),this, SLOT(cancelPressed()));
	connect(ui.AngleRangeP, SIGNAL(toggled(bool)), this, SLOT(updateTable()));
	connect(ui.AngleRangePM, SIGNAL(toggled(bool)), this, SLOT(updateTable()));
	
	ui.XAxisButton->setChecked(true);
	ui.ZHLineEdit->setEnabled(false);
	ui.ZKLineEdit->setEnabled(false);
	ui.ZLLineEdit->setEnabled(false);

}

void ReorientDialog::okPressed(void){
	
	for(int n=0;n<4;++n){
		if(ui.DataTable->isItemSelected(ui.DataTable->item(n,0))){
			selectedOrientation = true;
			angleX = ui.DataTable->item(n,0)->text().toDouble() * M_PI / 180;
			angleY = ui.DataTable->item(n,1)->text().toDouble() * M_PI / 180;
			angleZ = ui.DataTable->item(n,2)->text().toDouble() * M_PI / 180;
		}
	}
	
	done(OKClicked);
}

void ReorientDialog::printPressed(void){
	selectedOrientation = false;
	done(PrintClicked);
}

void ReorientDialog::cancelPressed(void){
	selectedOrientation = false;
	done(CancelClicked);
}


bool ReorientDialog::getSelectedOrientation(void){
	return selectedOrientation;
}

void ReorientDialog::getAngles(double *x, double *y, double *z){
	*x = angleX;
	*y = angleY;
	*z = angleZ;
}

void ReorientDialog::setLaue(lauegram* newLaue){
	laue = newLaue;
}

void ReorientDialog::setCrystal(Crystal* newCrystal){
	crystal = newCrystal;
}

void ReorientDialog::go(void){
	double h,k,l;
	
	h = ui.XHLineEdit->text().toDouble();
	k = ui.XKLineEdit->text().toDouble();
	l = ui.XLLineEdit->text().toDouble();
	
	setupTable();
	
	laue->reorientation.setUB(crystal->getUBunrotated());
	laue->reorientation.setPrimary(h,k,l);
	
	if(ui.ZHLineEdit->isEnabled()){
		double h2,k2,l2;
		h2 = ui.ZHLineEdit->text().toDouble();
		k2 = ui.ZKLineEdit->text().toDouble();
		l2 = ui.ZLLineEdit->text().toDouble();
		laue->reorientation.setSecondary(h2,k2,l2);
		laue->reorientation.calc(Reorientation::Zone);
	} else {
		int mode = Reorientation::XAxis;
		if(ui.XAxisRadioButton->isChecked()){
			mode = Reorientation::XAxis;
		}
		if(ui.YAxisRadioButton->isChecked()){
			mode = Reorientation::YAxis;
		}
		if(ui.ZAxisRadioButton->isChecked()){
			mode = Reorientation::ZAxis;
		}
		laue->reorientation.calc(mode);
	}
	
	updateTable();
	
	ui.okButton->setEnabled(true);
	ui.printButton->setEnabled(true);
}

void ReorientDialog::updateTable(void){
	double *angles;
	for(int i = 0;i<4;i++){
		for(int s = 0;s<2;s++){
			angles = laue->reorientation.getAngles(i, s);
			setTableItem(ui.DataTable, (i * 2) + s, 0, angles[0] * 180.0 / M_PI);
			setTableItem(ui.DataTable, (i * 2) + s, 1, angles[1] * 180.0 / M_PI);
			setTableItem(ui.DataTable, (i * 2) + s, 2, angles[2] * 180.0 / M_PI);
		}	
	}
}

void ReorientDialog::setupTable(void){
	
	ui.DataTable->setRowCount(8);
	ui.DataTable->setColumnCount(3);
	
	ui.DataTable->setColumnWidth(0,100);
	ui.DataTable->setColumnWidth(1,100);
	ui.DataTable->setColumnWidth(2,100);
	
	QStringList hHeaders, vHeaders;
	hHeaders << "Rot X" << "Rot Y" << "Rot Z";
	ui.DataTable->setHorizontalHeaderLabels(hHeaders);
	ui.DataTable->verticalHeader()->setVisible(false);
	ui.DataTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.DataTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void ReorientDialog::setTableItem(QTableWidget *table, int x, int y, double val) {
	
	if(val == 0.0){
		QTableWidgetItem *newItem = new QTableWidgetItem();
		newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		newItem->setBackgroundColor(QColor("lightgray"));
		table->setItem(x,y, newItem);
	} else {
		double newval = val;
		if(ui.AngleRangeP->isChecked() == TRUE){
			if(val < 0){
				newval = val + 360.0;
			} 
		} 
		QTableWidgetItem *newItem = new QTableWidgetItem(QString("%1").arg(newval,3,'f',2));
		newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		newItem->setTextColor(QColor("black"));
		newItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		table->setItem(x,y, newItem);
	}
}
