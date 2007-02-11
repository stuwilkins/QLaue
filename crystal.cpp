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

//#include <math.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "matrix.h"
#include "atom.h"
#include "crystal.h"
#include "spacegroup.h"
#include "xray.h"

Matrix Crystal::getGenerator(spacegroup *sg, int gennum) {
	Matrix gen(3,4);
	int n=0;
	
	for(int c=0 ; c<4 ;c++){
		for(int r=0 ; r<3 ;r++){
			if(c == 3){
				gen.Set(r,c,(double)sg->matrix[gennum][n] / (double)Crystal::STBF);
			} else {
				gen.Set(r,c,sg->matrix[gennum][n]);
			}
			n++;
		}
	}
	return gen;
}

Crystal::Crystal(void): 
B(3,3), Binv(3,3), 
U(3,3), Uinv(3,3),
UB(3,3), UBinv(3,3),
R(3,3), Rinv(3,3),
name(NULL)
{
	atoms = new Atom[2048];
	natoms = 0;
	natoms_sggen = 0;
	sg = &spacegroups[0];

	freeRotate = freeRotate;
	
	U.identity();
	R.identity();
	Rinv.identity();

	setLattice(5.4,5.4,5.4,M_PI / 2, M_PI/2, M_PI/2);
	return;
}

Crystal::Crystal(const Crystal& C):
B(3,3), Binv(3,3), 
U(3,3), Uinv(3,3),
UB(3,3), UBinv(3,3),
R(3,3), Rinv(3,3),
name(NULL)
{
	atoms = new Atom[2048];
	natoms = C.natoms;
	natoms_sggen = C.natoms_sggen;
	
	// Copy the atoms across
	for(int i=0;i<(C.natoms+C.natoms_sggen);i++)
		atoms[i] = C.atoms[i];
	
	// Now the name
	
	if(C.name == NULL){
		name = NULL;
	} else {
		name = new char[strlen(C.name)];
		strcpy(name,C.name);
	}
	
	// Lattice parameters
	
	a = C.a;
	b = C.b;
	c = C.c;
	alpha = C.alpha;
	beta = C.beta;
	gamma = C.gamma;
	
	as = C.as;
	bs = C.bs;
	cs = C.cs;
	alphas = C.alphas;
	betas = C.betas;
	gammas = C.gammas;
	
	sinalphas = C.sinalphas;
	sinbetas = C.sinbetas;
	singammas = C.singammas;
	
	cosalphas = C.cosalphas;
	cosbetas = C.cosbetas;
	cosgammas = C.cosgammas;
	
	volume = C.volume;
	volumes = C.volumes;
	
	sg = C.sg;
	
	freeRotate = C.freeRotate;
	
	B = C.B;
	Binv = C.Binv;	
	U = C.U;
	Uinv = C.Uinv;
	UB = C.UB;
	UBinv = C.UBinv;
	R = C.R;
	Rinv = C.Rinv;
}

Crystal::~Crystal(void) {
	delete [] atoms;
	if(name != NULL)
		delete [] name;
	return;
}

Crystal& Crystal::operator=(const Crystal &C) {
	// Copy the atoms across
	
	natoms = C.natoms;
	natoms_sggen = C.natoms_sggen;
	
	for(int i=0;i<(C.natoms+C.natoms_sggen);i++)
		atoms[i] = C.atoms[i];
	
	// Now the name
	
	if(C.name == NULL){
		name = NULL;
	} else {
		name = new char[strlen(C.name)];
		strcpy(name,C.name);
	} 
	
	// Lattice parameters
	
	a = C.a;
	b = C.b;
	c = C.c;
	alpha = C.alpha;
	beta = C.beta;
	gamma = C.gamma;
	
	as = C.as;
	bs = C.bs;
	cs = C.cs;
	alphas = C.alphas;
	betas = C.betas;
	gammas = C.gammas;
	
	sinalphas = C.sinalphas;
	sinbetas = C.sinbetas;
	singammas = C.singammas;
	
	cosalphas = C.cosalphas;
	cosbetas = C.cosbetas;
	cosgammas = C.cosgammas;
	
	volume = C.volume;
	volumes = C.volumes;
	
	sg = C.sg;
	
	freeRotate = C.freeRotate;
	
	B = C.B;
	Binv = C.Binv;
	U = C.U;
	Uinv = C.Uinv;
	UB = C.UB;
	UBinv = C.UBinv;
	R = C.R;
	Rinv = C.Rinv;
	return *this;
}

int Crystal::getLatticeType(void){
	//return lattice_type;
	return 0;
}

void Crystal::calc(void) {
	calcRlattice();
	calcUB();
}

void Crystal::setLattice(double newa,  double newb, double newc,
				double newalpha, double newbeta, double newgamma){
	a = newa;
	b = newb;
	c = newc;
	
	alpha = newalpha;
	beta = newbeta;
	gamma = newgamma;
	
	calcRlattice();
	calcUB();
}

double Crystal::getLatticeA(void){
	return a;
}

double Crystal::getLatticeB(void){
	return b;
}

double Crystal::getLatticeC(void){
	return c;
}

double Crystal::getLatticeAlpha(void){
	return alpha;
}

double Crystal::getLatticeBeta(void){
	return beta;
}

double Crystal::getLatticeGamma(void){
	return gamma;
}

int Crystal::getNAtoms(void){
	return natoms + natoms_sggen;
}

int Crystal::getNRootAtoms(void){
	return natoms;
}

void Crystal::delAllAtoms(void){
	natoms = 0;
	natoms_sggen = 0;
}

Atom * Crystal::getAtom(int i){
	return &(atoms[i]);
}

Matrix Crystal::getAtom4(int i){
	Matrix pos(4,1);
	pos.Set(0,0,atoms[i].getXPos());
	pos.Set(1,0,atoms[i].getYPos());
	pos.Set(2,0,atoms[i].getZPos());
	pos.Set(3,0,1.0);
	return pos;
}

void Crystal::calcRlattice(void) {

	double abc = a*b*c;
	
	volume = sqrt( 1 - pow(cos(alpha),2) 
				  - pow(cos(beta),2) - pow(cos(gamma),2) 
				  + (2*cos(alpha)*cos(beta)*cos(gamma)));
	
	as = /*2 * M_PI * */sin(alpha) / ( a * volume);
	bs = /*2 * M_PI * */sin(beta) / ( b * volume);
	cs = /*2 * M_PI * */sin(gamma) / ( c * volume);
	
	sinalphas = (volume / (sin(beta) * sin(gamma)));
	sinbetas = (volume / (sin(alpha) * sin(gamma)));
	singammas = (volume / (sin(alpha) * sin(beta)));
	
	cosalphas = (cos(beta)*cos(gamma) - cos(alpha)) / (sin(beta) * sin(gamma));
	cosbetas = (cos(alpha)*cos(gamma) - cos(beta)) / (sin(alpha) * sin(gamma));
	cosgammas = (cos(alpha)*cos(gamma) - cos(gamma)) / (sin(beta) * sin(beta));
	
	volume*=abc;
	
	volumes = /*2 * M_PI*/ 1 / volume;
}

void Crystal::calcUB(void){
	B.Set(0,0,as);
	B.Set(1,0,0);
	B.Set(2,0,0);
	
	B.Set(0,1,bs * cosgammas);
	B.Set(1,1,bs * singammas);
	B.Set(2,1,0);
	
	B.Set(0,2,cs * cosbetas);
	B.Set(1,2,-1 * cs * sinbetas * cos(alpha));
	B.Set(2,2,/*2 * M_PI*/ 1 / c);
	
	Binv = B.inv();
	Uinv = U.transpose();
	UB = U * B;
	UBinv = Binv * Uinv;
}

void Crystal::setU(const Matrix newU){
	U = newU;
	// Zero the rotations and R matrix
	gonioX = 0; gonioY = 0; gonioZ = 0;
	R.identity() ; Rinv.identity();
	calcUB();
}

void Crystal::setU(double h1, double k1, double l1,
				   double h2, double k2, double l2){

	// Set the orientation matrix like a UB
	// h1,k1,l1 make the primary (x) direction while
	// h2,k2,l2 make the secondary (z) direction
	
	Matrix Primary(h1,k1,l1);
	Matrix Secondary(h2,k2,l2);
	setU(Primary,Secondary);
}
	
void Crystal::setU(Matrix Primary, Matrix Secondary){
	
	Matrix X_hat(3,1), Z_hat(3,1), Y_hat(3,1);
	
	// Convert into crystal cartezian
	
	Primary = B * Primary;
	Secondary = B * Secondary;
	
	X_hat = Primary * (1.0 / Primary.mag());
	Z_hat = Secondary * (1.0 / Secondary.mag());
	
	Y_hat = Z_hat ^ X_hat;
	Z_hat = X_hat ^ Y_hat;
	
	Matrix newU(3,3);
	
	for(int i = 0; i < 3 ; i++){
		newU.Set(0,i,X_hat.Get(i,0));
		newU.Set(1,i,Y_hat.Get(i,0));
		newU.Set(2,i,Z_hat.Get(i,0));
	}
	
	setU(newU);
}	

void Crystal::setU(Matrix Primary, Matrix Secondary, Matrix PrimaryHKL, Matrix SecondaryHKL){
	Matrix Tc(3,3), Td(3,3), newU(3,3);
	Matrix Tc2(3,1), Tc3(3,1);
	Matrix Td2(3,1), Td3(3,1);
	
	Matrix vec1 = PrimaryHKL;
	Matrix vec2 = SecondaryHKL;
	
	vec1 = getB() * vec1;
	vec1 = vec1 / vec1.mag();
	vec2 = getB() * vec2;
	vec2 = vec2 / vec2.mag();
	
	Tc3 = vec1 ^ vec2;
	Tc3 = Tc3 / Tc3.mag();
	Tc2 = Tc3 ^ vec1;
	Tc2 = Tc2 / Tc2.mag();
	
	Primary = Primary / Primary.mag();
	Secondary = Secondary / Secondary.mag();
	
	Td3 = Primary ^ Secondary;
	Td3 = Td3 / Td3.mag();
	Td2 = Td3 ^ Primary;
	Td2 = Td2 / Td2.mag();
	
	for(int i = 0 ; i < 3 ; i++ ){
		// Load up the matricies Tc and Td;
		Tc.Set(i,0,vec1.Get(i,0));
		Tc.Set(i,1,Tc2.Get(i,0));
		Tc.Set(i,2,Tc3.Get(i,0));
		
		Td.Set(i,0,Primary.Get(i,0));
		Td.Set(i,1,Td2.Get(i,0));
		Td.Set(i,2,Td3.Get(i,0));
	}
	
	newU = Td * Tc.transpose();
	setU(newU);
}

Matrix Crystal::toQ(double x, double y, double z){
	Matrix miller(3,1);
	Matrix Q(3,1);
	
	miller.Set(0,0,x);
	miller.Set(1,0,y);
	miller.Set(2,0,z);
	
	Q = B * miller;
	return Q;
}

void Crystal::addAtom(int zn, double x, double y, double z) {
	atoms[natoms].setPos(x,y,z);
	atoms[natoms].setZ(zn);
	atoms[natoms].setRootAtom(-1);
	natoms++;
}

complex<double> Crystal::calcXSFactor(int h, int k, int l, double sinthoverlambda, 
									  double lambda){
	return Crystal::calcXSFactor((double)h, (double)k, (double)l, sinthoverlambda, lambda);
}

complex<double> Crystal::calcXSFactor(double h, double k, double l, double sinthoverlambda, 
									  double lambda){
	complex<double> sfactor(0,0);
	for(int i=0;i<(natoms + natoms_sggen);i++){
		complex<double> f1f2 = XRay::getXf1f2(atoms[i].getZ(), lambda);
		//cerr << "f1f2 = " << f1f2 << endl;
		double f0 = XRay::getXf0(atoms[i].getZ(), sinthoverlambda);
		double f0_0 = XRay::getXf0(atoms[i].getZ(), 0);
		//cerr << f0_0 << endl;
		sfactor = sfactor + (atoms[i].getPhase(h,k,l) * (f1f2 + f0 - f0_0));
		//sfactor = sfactor + (atoms[i].getPhase(h,k,l) * f0_0);
	}
	//cerr << "(" << h << " " << k << " " << l <<") sfactor = " << sfactor << endl; 
	return sfactor;
}

bool Crystal::isForbidden(double h, double k, double l){
	complex<double> sfactor(0,0);
	for(int i=0;i<(natoms + natoms_sggen);i++){
		double f0_0 = XRay::getXf0(atoms[i].getZ(), 0);
		sfactor = sfactor + (atoms[i].getPhase(h,k,l) * f0_0);
	}
	
	double inten = (sfactor * conj(sfactor)).real();
	
	if(inten < 0.00001)
		return true;
	else
		return false;
}

Matrix Crystal::getB(void){
	return B;
}

Matrix Crystal::getBinv(void){
	return Binv;
}

Matrix Crystal::getU(void){
	return R * U;
}

Matrix Crystal::getUinv(void){
	return Uinv * Rinv;
}

Matrix Crystal::getUB(void){
	return R * UB;
}

Matrix Crystal::getUBunrotated(void){
	return UB;
}

Matrix Crystal::getUBinv(void){
	return UBinv * Rinv;
}

// Rotation routines

void Crystal::setFreeRotate(bool state){
	freeRotate = state;

	if(freeRotate){
		// Free rotate so rotate the actual U Matrix
		// by the current gonio angles
		Matrix X(3,3),Y(3,3),Z(3,3);
		X = X.rotateX(gonioX);
		Y = Y.rotateY(gonioY);
		Z = Z.rotateZ(gonioZ);
		gonioX = 0; gonioY = 0; gonioZ = 0;
		R.identity() ; Rinv.identity();
		U = Z * Y * X * U;
		calcUB();
	}
}

void Crystal::rotateBy(double a, double b, double c){
	if(freeRotate) {
		Matrix X(3,3),Y(3,3),Z(3,3);
		X = X.rotateX(a);
		Y = Y.rotateY(b);
		Z = Z.rotateZ(c);
		U = Z * Y * X * U;
		calcUB();
	} else {
		gonioX = gonioX + a; 
		gonioY = gonioY + b; 
		gonioZ = gonioZ + c; 
		setGoniometer();
	}
}

void Crystal::rotateTo(double a, double b, double c){
	if(freeRotate){
		Matrix X(3,3),Y(3,3),Z(3,3);
		X = X.rotateX(a);
		Y = Y.rotateY(b);
		Z = Z.rotateZ(c);
		U = Z * Y * X * U;
		calcUB();
	} else {
		gonioX = a; 
		gonioY = b; 
		gonioZ = c; 
		setGoniometer();
	}
}

void Crystal::setGoniometer(void){
	Matrix X(3,3),Y(3,3),Z(3,3);
	X = X.rotateX(gonioX);
	Y = Y.rotateY(gonioY);
	Z = Z.rotateZ(gonioZ);
	
	R = Z * Y * X;
	Rinv = R.transpose();
}



void Crystal::rotateAboutBy(Matrix axis, double angle){
	// First we need to make the Tphi matricies from
	Matrix R(3,3), Tc(3,3);
	
	Matrix axis2(3,1), Tc2(3,1), Tc3(3,1);
	
	axis = axis / axis.mag();
	
	// choose either x or y axis
	// (whichever is greater in angle)
	
	if(axis.dot(Matrix(0,1,0)) > axis.dot(Matrix(0,0,1)))
		axis2 = Matrix(0,0,1);
	else
		axis2 = Matrix(0,1,0);
	 
	Tc3 = axis ^ axis2;
	Tc3 = Tc3 / Tc3.mag();
	Tc2 = Tc3 ^ axis;
	Tc2 = Tc2 / Tc2.mag();
										
	for(int i = 0 ; i < 3 ; i++ ){
		// Load up the matricies Tc and Td;
		Tc.Set(i,0,axis.Get(i,0));
		Tc.Set(i,1,Tc2.Get(i,0));
		Tc.Set(i,2,Tc3.Get(i,0));
	}
			
	Matrix X(3,3);
	X.identity();
	X = X.rotateX(angle);
	
	R.identity();
	R = Tc * X * Tc.transpose();

	U = R * U;
	calcUB();
}

// ***************************************************************************************

spacegroup* Crystal::getSpaceGroup(void){
	return sg;
}

bool Crystal::setSpaceGroupNum(const int sgnum){
	sg = &spacegroups[0];
	for(int n=0; sg->Name != NULL; n++){
		if(sg->number == sgnum)
			return true;
		sg = &spacegroups[n];
	}
	return false;
}

void Crystal::setSpaceGroup(const int sgnum){
	sg = &spacegroups[sgnum];
}

bool Crystal::setSpaceGroup(const char *name){
	int n = 0;
	for(sg = spacegroups; sg->Name != NULL; ++sg){
		if(!strcmp(name, sg->HallName)){
			return true;
		}
		n++;
	}
	return false;	
}

bool Crystal::setSpaceGroupIUC(const char *iucname){
	int n = 0;
	for(sg = spacegroups; sg->Name != NULL; ++sg){
		if(!strcmp(iucname, sg->Name)){
			return true;
		}
		n++;
	}
	return false;	
}

void Crystal::spaceGroupGenerate(void){
	sgTransform();
}

void Crystal::sgTransform(void){
	// Cycle over all atoms
	Matrix newpos(3,1);
	Matrix atompos(4,1);
	Matrix op(3,4);
	
	bool flag = true;
	while(flag == true){
		flag = false;
		//cerr << "Cycling over " << sg->ngenerators << " generators" << endl;
		for(int n=0;n<sg->ngenerators;n++){
			for(int a=0; a<(natoms) ; a++){
				atompos = getAtom4(a);
				op = getGenerator(sg,n);
				//cerr << "Operator " << n << endl;
				//cerr << op;
				newpos = op * atompos;
				if(sgInsertAtom(atoms[a], newpos)){
					atoms[natoms + natoms_sggen - 1].setRootAtom(a);
					//cerr << "Crystal::sgTransform() : *** Inserted atom ***" << endl;
					flag = true;
				}
			}
		}
	}
	
	// Now apply centering 
	
	//cerr << "crystal::sgGenerate() : Lattice type = " << sg->LatticeType << endl;
	
	switch(sg->LatticeType) {
		case 'P': break;
		case 'I': sgBodyCenter(); break;
		case 'F': sgFaceCenter(); break;
		case 'A': sgACenter(); break;
		case 'B': sgBCenter(); break;
		case 'C': sgCCenter(); break;
		case 'R': sgRhomb(); break;
		default :
			cerr << "crystal::sgGenerate() : Unknown lattice centering" << endl;
			return;
			break;
	} 
}

bool Crystal::sgInsertAtom(const Atom a, const Matrix pos){
	// Check for stupidity and	
	// If we have a negative number then add 1 to it.
	Matrix newpos(pos);
	for(int i=0;i<3;i++){
		if((newpos.Get(0,i) >= 1) && (newpos.Get(0,i) < 2))
			newpos.Set(0,i,newpos.Get(0,i) - 1);
		if((newpos.Get(0,i) < 0) && (newpos.Get(0,i) >= -1))
			newpos.Set(0,i,newpos.Get(0,i) + 1);
		if((newpos.Get(0,i) < -1) && (newpos.Get(0,i) >= -2))
			newpos.Set(0,i,newpos.Get(0,i) + 2);
	}
	
	// Is there an atom already in this place ?
	// Define an accuracy based on the lattice parameter
	
	double xacc = (0.25 / getLatticeA());
	double yacc = (0.25 / getLatticeB());
	double zacc = (0.25 / getLatticeC());
	
	for(int n=0;n<(natoms + natoms_sggen);++n){
		if((abs(newpos.Get(0,0) - atoms[n].getXPos()) < xacc) &&
		   (abs(newpos.Get(0,1) - atoms[n].getYPos()) < yacc) &&
		   (abs(newpos.Get(0,2) - atoms[n].getZPos()) < zacc)) {
			//cerr << "Crystal::sgInsertAtom() : Atom located at this pos" << endl;
				return false;
		}
	}
	
	// Ok so we can insert an atom in this location
	
	atoms[natoms + natoms_sggen] = a;
	atoms[natoms + natoms_sggen].setPos(newpos);
	natoms_sggen++;
	return true;
}

void Crystal::sgFaceCenter(void){
	Matrix v1(0.5, 0.5, 0);
	Matrix v2(0, 0.5, 0.5);
	Matrix v3(0.5, 0, 0.5);
	sgApplyCenter(v1);
	sgApplyCenter(v2);
	sgApplyCenter(v3);
}

void Crystal::sgBodyCenter(void){
	Matrix v(0.5, 0.5, 0.5);
	sgApplyCenter(v);
}

void Crystal::sgACenter(void){
	Matrix v(0, 0.5, 0.5);
	sgApplyCenter(v);
}

void Crystal::sgBCenter(void){
	Matrix v(0.5, 0, 0.5);
	sgApplyCenter(v);
}

void Crystal::sgCCenter(void){
	Matrix v(0.5, 0.5, 0);
	sgApplyCenter(v);
}

void Crystal::sgRhomb(void){
	Matrix v1(2.0/3.0, 1.0/3.0,1.0/3.0);
	Matrix v2(1.0/3.0, 2.0/3.0,2.0/3.0);
	sgApplyCenter(v1);
	sgApplyCenter(v2);
}

void Crystal::sgApplyCenter(Matrix v){
	int max = (natoms + natoms_sggen);
	for(int n=0;n<max;n++){
		sgInsertAtom(atoms[n],atoms[n].getPos() + v); 
	}	
}

// ***************************************************************************************

void Crystal::setName(const char *txt){
	// First free up the memory
	
	if(name != NULL)
		delete [] name;
	
	name = new char[strlen(txt)];
	
	strcpy(name,txt);
}

char* Crystal::getName(void){
	return name;
}

// ***************************************************************************************

// Calculate absorption (mu) 

double Crystal::xRayAbsorptionCoeff(double lambda){
	double mu = 0;
	for(int i=0; i<(natoms + natoms_sggen); ++i){
		complex<double> f1f2 = XRay::getXf1f2(atoms[i].getZ(), lambda);
		mu = mu + f1f2.imag();
	}
	
	mu = mu * -2.0 * lambda * 1e-10 * R_ZERO / (volume / 1e30);
	//cerr << "mu crystal = " << mu << endl;
	return mu;
}