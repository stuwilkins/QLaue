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

#ifndef _CRYSTAL
#define _CRYSTAL 1

#include "atom.h"
#include "matrix.h"

typedef struct spacegroup {
	int index;
	int number;
	char *Name;
	char *Extn;
	char *HallName;
	char LatticeType;
	int XtalSystem;
	int Centric;
	int ngenerators;
	int matrix[192][12];
} spacegroup;


class Crystal {
private:
	// Atoms ( from Atom class)
	
	Atom *atoms;
	int natoms;
	int natoms_sggen;
	
	// Lattice parameters
	
	double a,b,c;
	double alpha,beta,gamma;
	
	// Cell angles
	
	double as,bs,cs;
	double alphas,betas,gammas;
	double sinalphas,sinbetas,singammas;
	double cosalphas,cosbetas,cosgammas;
	
	// Cell Volume
	
	double volume;
	double volumes;
	
	// Spacegroup
	
	spacegroup *sg;
	
	// B Matrix for coordinate transform
	Matrix B;
	Matrix Binv;
	Matrix U;
	Matrix Uinv;
	Matrix UB;
	Matrix UBinv;
	Matrix R;
	Matrix Rinv;
	
	// Angles for gonio
	
	bool freeRotate;
	double gonioX;
	double gonioY;
	double gonioZ;
	
	// Place to store a name for the crystal
	
	char *name;
	
	// Spacegroup Centering
	
	void sgFaceCenter(void);
	void sgBodyCenter(void);
	void sgACenter(void);
	void sgBCenter(void);
	void sgCCenter(void);
	void sgRhomb(void);
	void sgApplyCenter(Matrix v);
	
	Matrix getGenerator(spacegroup *sg, int gennum);
	bool sgInsertAtom(const Atom a, const Matrix pos);
	void sgTransform(void);
	Matrix getAtom4(int i);
	
	void setGoniometer(void);
	
	void calcUB(void);
	void calcRlattice(void);
	
public:	
	// Constructors	
		
	Crystal();
	Crystal(const Crystal& C);
	~Crystal();
	
	// Overloaded operators
	
	Crystal& operator=(const Crystal &C);
	
	// Force calculation of crystal
	
	void calc(void);
	
	// Latice operations
	
	void setLattice(double a,  double b, double c,
					double alpha, double beta, double gamma);
	
	double getLatticeA(void);
	double getLatticeB(void);
	double getLatticeC(void);
	double getLatticeAlpha(void);
	double getLatticeBeta(void);
	double getLatticeGamma(void);
	int getLatticeType(void);
	int getNAtoms(void);
	int getNRootAtoms(void);
	Atom* getAtom(int i);
	void delAllAtoms(void);
	
	void setName(const char *txt);
	char* getName(void);
	
	// Lattice Calculations
	
	Matrix toQ(double x, double y, double z);
	Matrix xAxis(void)			{ return Binv * Uinv * Rinv * Matrix(1,0,0); }
	Matrix yAxis(void)			{ return Binv * Uinv * Rinv * Matrix(0,1,0); }
	Matrix zAxis(void)			{ return Binv * Uinv * Rinv * Matrix(0,0,1); }
							
	// Crystal Orientation
	
	void setU(double h1, double k1, double l1,
			  double h2, double k2, double l2);
	void setU(const Matrix newU);
	void setU(Matrix Primary, Matrix Secondary);
	void setU(Matrix Primary, Matrix Secondary, Matrix PrimaryHKL, Matrix SecondaryHKL);
	
	// Rotation Routines
	
	void rotateBy(double a, double b, double c);
	void rotateTo(double a, double b, double c);
	void rotateAboutBy(Matrix axis, double angle);
	
	void setFreeRotate(bool state);
	bool getFreeRotate(void)		{ return freeRotate; }
	double getGonioX(void)			{ return gonioX; }
	double getGonioY(void)			{ return gonioY; }
	double getGonioZ(void)			{ return gonioZ; }
	
	// Atom operations
	
	void Crystal::addAtom(int zn, double x, double y, double z);
	complex<double> calcXSFactor(double h, double k, double l, double sinthoverlambda, 
								 double lambda);
	complex<double> calcXSFactor(int h, int k, int l, double sinthoverlambda,
								 double lambda);
	bool isForbidden(double h, double k, double l);
	
	// Access to crystal matricies
	
	Matrix getB(void);
	Matrix getBinv(void);
	Matrix getU(void);
	Matrix getUinv(void);
	Matrix getUB(void);
	Matrix getUBunrotated(void);
	Matrix getUBinv(void);
	
	// Spacegroup Access Routines
	
	void setSpaceGroup(const int sgnum);
	bool setSpaceGroup(const char *name);
	bool setSpaceGroupIUC(const char *iucname);
	bool setSpaceGroupNum(const int sgnum);
	spacegroup* getSpaceGroup(void);
	void spaceGroupGenerate(void);
	
	// Access to x-ray properties 
	
	double xRayAbsorptionCoeff(double lambda);
	
	// enum's for types of lattice
	
	enum {
		Unknown  = 0,
		Triclinic = 1,
		Monoclinic = 2,
		Orthorhombic = 3,
		Tetragonal = 4,
		Trigonal = 5,
		Hexagonal =  6,
		Cubic = 7
	};
	
	// and for basis of Seitz matrixies.
	
	enum {
		STBF = 12, // Seitz           Matrix Translation Base Factor 
		CRBF = 12, // Change of Basis Matrix Rotation    Base Factor
		CTBF = 72 // Change of Basis Matrix Translation Base Factor
	};
};

#endif
