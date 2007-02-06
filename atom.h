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

#ifndef _ATOM_H
#define _ATOM_H 1

/*
 Class Definitions for Atoms
 */
#include <complex>
#include "matrix.h"

const char ZtoName[][3] = {
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

typedef struct ScatFactf1f2 {
	int Z;
	double data[2000][3];
	int nvalues;
} f1f2;

using std::complex;
typedef complex<double> dcmplx;

class Atom {
private:
	// Atomic Number
	int Z;
	// Position in unit cell
	double a, b, c;
	// Valence
	int C;
	// Root or not
	int root_atom;
public:
	void setPos(double x, double y, double z);
	void setPos(const Matrix pos);
	double getXPos(void);
	double getYPos(void);
	double getZPos(void);
	
	int rootAtom(void) const	{ return root_atom; }
	void setRootAtom(int r)		{ root_atom = r; }
	
	const char* getZName(void);
	Matrix getPos(void);
	void setZ(int z);
	int getZ(void);
	const char* getZname(void);
	dcmplx getPhase(double h, double k, double l);
	Atom operator=(const Atom A);
};

#endif