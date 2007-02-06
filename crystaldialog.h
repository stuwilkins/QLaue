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

#ifndef _CRYSTALDIALOG_H
#define _CRYSTALDIALOG_H 1

#include <QtGui>
#include "ui_crystaldialog.h"
#include "crystal.h"

const char the_elements[][3] = {
	"H "  ,  "He"  ,  "Li"  ,  "Be"  ,  "B "  ,
	"C "  ,  "N "  ,  "O "  ,  "F "  ,  "Ne"  ,
	"Na"  ,  "Mg"  ,  "Al"  ,  "Si"  ,  "P "  ,
	"S "  ,  "Cl"  ,  "Ar"  ,  "K "  ,  "Ca"  ,
	"Sc"  ,  "Ti"  ,  "V "  ,  "Cr"  ,  "Mn"  ,
	"Fe"  ,  "Co"  ,  "Ni"  ,  "Cu"  ,  "Zn"  ,
	"Ga"  ,  "Ge"  ,  "As"  ,  "Se"  ,  "Br"  ,
	"Kr"  ,  "Rb"  ,  "Sr"  ,  "Y "  ,  "Zr"  ,
	"Nb"  ,  "Mo"  ,  "Tc"  ,  "Ru"  ,  "Rh"  ,
	"Pd"  ,  "Ag"  ,  "Cd"  ,  "In"  ,  "Sn"  ,
	"Sb"  ,  "Te"  ,  "I "  ,  "Xe"  ,  "Cs"  ,
	"Ba"  ,  "La"  ,  "Ce"  ,  "Pr"  ,  "Nd"  ,
	"Pm"  ,  "Sm"  ,  "Eu"  ,  "Gd"  ,  "Tb"  ,
	"Dy"  ,  "Ho"  ,  "Er"  ,  "Tm"  ,  "Yb"  ,
	"Lu"  ,  "Hf"  ,  "Ta"  ,  "W "  ,  "Re"  ,
	"Os"  ,  "Ir"  ,  "Pt"  ,  "Au"  ,  "Hg"  ,
	"Tl"  ,  "Pb"  ,  "Bi"  ,  "Po"  ,  "At"  ,
	"Rn"  ,  "Fr"  ,  "Ra"  ,  "Ac"  ,  "Th"  ,
	"Pa"  ,  "U "  ,  "Np"  ,  "Pu"  ,  "Am"  ,
	"Cm"  ,  "Bk"  ,  "Cf"  
};

class CrystalDialog : public QDialog
{
	Q_OBJECT
public:
	CrystalDialog();
	void setCrystal(Crystal *crystal);
	void getCrystal(Crystal *crystal);
private slots:
	void latticeTypeChanged(bool state);	
	void latticeAChanged(const QString newval);
	void setSpacegroupBox(int crystaltype);
private:
	Ui::CrystalDialog ui;
	int lattice_type;
	int SpacegroupBoxIndex[1000];
};

#endif