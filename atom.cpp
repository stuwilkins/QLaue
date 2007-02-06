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

#include <complex>
#include "matrix.h"
#include "atom.h"
#include "crystal.h"

#define HC_OVER_E 12.39842

Atom Atom::operator=(const Atom A){
	a = A.a; b = A.b; c = A.c;
	C = A.C; Z = A.Z;
	root_atom = A.root_atom;
	return *this;
}

void Atom::setPos(double x, double y, double z){
	a = x;
	b = y;
	c = z;
}

void Atom::setPos(const Matrix pos){
	a = pos.Get(0,0);
	b = pos.Get(1,0);
	c = pos.Get(2,0);
}

double Atom::getXPos(void){
	return a;
}

double Atom::getYPos(void){
	return b;
}

Matrix Atom::getPos(void){
	Matrix pos(3,1);
	
	pos.Set(0,0,a);
	pos.Set(0,1,b);
	pos.Set(0,2,c);
	
	return pos;
}

double Atom::getZPos(void){
	return c;
}

const char* Atom::getZName(void){
	return ZtoName[Z-1];
}

int Atom::getZ(void){
	return Z;
}

void Atom::setZ(int z){
	Z = z;
}

dcmplx Atom::getPhase(double h, double k, double l){
	double dot = (h*a) + (k*b) + (l*c);
	complex<double> B = exp(complex<double> (0,dot * 2.0 * M_PI));
	//cerr << B;
	return B;
}