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

#include <QApplication>
#include <algorithm>
using namespace std;

#include "matrix.h"
#include "atom.h"
#include "crystal.h"
#include "laue.h"
#include "xray.h"

LaueSpot::LaueSpot(void){
	fromCharateristic = false;
	return;
}

double LaueSpot::H(void){
	return h;
}

double LaueSpot::K(void){
	return k;
}

double LaueSpot::L(void){
	return l;
}

double LaueSpot::X(void){
	return x;
}

double LaueSpot::Y(void){
	return y;
}

double LaueSpot::I(void){
	return inten;
}

double LaueSpot::detI(void){
	return detInten;
}

void LaueSpot::setI(double i){
	inten = i;
}

void LaueSpot::setDetI(double i){
	detInten = i;
}

int LaueSpot::getMaxdo(void){
	return multiplicity;
}

void LaueSpot::setXYIN(double xx, double yy, double ii, int n){
	x = xx; y = yy;
	inten = ii;
	multiplicity = n;
}

void LaueSpot::setHKL(double hh, double kk, double ll){
	h = hh; k = kk; l = ll;
}

LaueOrientations::LaueOrientations():
U(3,3), P(3,1), S(3,1)
{
	U.identity();
	P.identity();
	S.identity();
}

LaueOrientations::LaueOrientations(const LaueOrientations& L):
U(3,3), P(3,1), S(3,1)
{
	U = L.U;
	P = L.P;
	S = L.S;
	numMatches = L.numMatches;
	patternError = L.patternError;
}

lauegram::lauegram() {
	spot = new LaueSpot[65000];
	init();
}

lauegram::lauegram(const lauegram& L) :
XRay() {
	spot = new LaueSpot[65000];
	
	nspots = L.nspots;
	debug = L.debug;
	fsDistance = L.fsDistance;
	inten_thresh = L.inten_thresh;
	
	maxKvec = L.maxKvec;
	minKvec = L.minKvec;
	
	detlevel = L.detlevel;
	detsaturation = L.detsaturation;
}

void lauegram::init(void) {
	
	nspots = 0;	
	fsDistance = 35; // mm
	
	debug = 0;
	
	// Set the K vector range to zero (no spots)
	
	minKvec = 0;
	maxKvec = 0;
	
	// Intensity threshold (to stop rounding errors)
	
	inten_thresh = 1; // Electrons
	
	// Intensity modifiers
	
	setIntensityCorrection(1023);
	
	// Detector Levels
	
	detlevel = 0.02;
	detsaturation = 0.45;

	calc_precision = 5;
	
	tube_z = XRay::elementNumber("Mo");
	
	calc_intensities = true;
	
	// Maximum 
	
	indexingMaxH = 4;
	indexingMaxK = 4;
	indexingMaxL = 4;
	
	indexingSpotTol = 5;
	indexingAngularTol = 1.5 * 4 * atan(1) / 180;
}

lauegram::~lauegram() {
	delete [] spot;
}

lauegram& lauegram::operator=(const lauegram &L){

	nspots = L.nspots;
	debug = L.debug;
	fsDistance = L.fsDistance;
	inten_thresh = L.inten_thresh;
	
	maxKvec = L.maxKvec;
	minKvec = L.minKvec;
	
	detlevel = L.detlevel;
	detsaturation = L.detsaturation;
	
	for(int i=0;i<nspots;i++){
		spot[i] = L.spot[i];
	}
	
	return *this;
}

void lauegram::setDet(double dl, double ds){
	detlevel = dl;
	detsaturation = ds;
}

double lauegram::getDetLevel(void){
	return detlevel;
}

void lauegram::setDetLevel(double dl){
	detlevel = dl;
}

double lauegram::getDetSatLevel(void){
	return detsaturation;
}

void lauegram::setDetSatLevel(double ds){
	detsaturation = ds;
}

double lauegram::detLevel(void){
	return detlevel;
}

double lauegram::detSaturation(void){
	return detsaturation;
}

void lauegram::setDebug(int value){
	debug = value;
}

void lauegram::setDistance(double d){
	fsDistance = d;
}

double lauegram::getDistance(void) {
	return fsDistance;
}

int lauegram::getNspots(void) {
	return nspots;
}

double lauegram::maxSpotInten(void){
	double maxinten = 0;
	
	for(int i = 0; i < nspots ; i++){
		if(maxinten < spot[i].I())
			maxinten = spot[i].I();
	}
	
	return maxinten;
}

double lauegram::maxSpotInten(double minx, double maxx, double miny, double maxy){
	double maxinten = 0;
	
	for(int i = 0; i < nspots ; i++){
		if((spot[i].X() >= minx) && (spot[i].X() <= maxx) &&
		   (spot[i].Y() >= miny) && (spot[i].Y() <= maxy)){
			if(maxinten < spot[i].I()){
				maxinten = spot[i].I();
			}
		}
	}
	
	return maxinten;
}

void lauegram::setLambdaRange(double min, double max){
	minKvec = 1 / max;
	maxKvec = 1 / min;
}

double lauegram::getLambdaRangeMax(void){
	return 1 / minKvec;
}

double lauegram::getLambdaRangeMin(void){
	return 1 / maxKvec;
}

bool lauegram::calc(Crystal *crystal, bool* abort, int *spotcount) {
	// Calculate the lauegram from a given 
	bool abortcheck = !(abort == 0);
	
	// Set the maximum HKL from max lambda
	if(debug){
		if(intensityCorrection() & IntensitySelfAbsorption)
			cerr << "lauegram::calc() : Calc self abs" << endl;
		if(intensityCorrection() & IntensityXRayTube)
			cerr << "lauegram::calc() : Calc tube responce" << endl;
		if(intensityCorrection() & IntensityLorentz)
			cerr << "lauegram::calc() : Calc Lorentz factor" << endl;
		if(intensityCorrection() & IntensityPolarization)
			cerr << "lauegram::calc() : Calc Poln Factor" << endl;
	}
	if(crystal == 0)
		return false;
	
	int maxH = (int)(2.0 * maxKvec * crystal->getLatticeA() / calc_precision);
	int maxK = (int)(2.0 * maxKvec * crystal->getLatticeB() / calc_precision);
	int maxL = (int)(2.0 * maxKvec * crystal->getLatticeC() / calc_precision);
	
	if(debug){
		cerr << "lauegram::calc() : maxKvec = " << maxKvec << endl;
		cerr << "lauegram::calc() : Max H = " << maxH << endl;
		cerr << "lauegram::calc() : Max K = " << maxK << endl;
		cerr << "lauegram::calc() : Max L = " << maxL << endl;
	}
	
	int spotn = 0;
	
	for(int h=0; h<=maxH; h++){
		for(int k=0; k<=maxK; k++){
			for(int l=0; l<=maxL; l++){
				// For Qt applications we need to release control 
				// to the qt event que.
				
				//qApp->processEvents();
			
				int mask = ((h == 0) ? 1 : 0);
				mask = mask << ((k == 0) ? 1 : 0);
				mask = mask << ((l == 0) ? 1 : 0);
				
				for(int n=0;n<=7;n++){
					if( n & mask)
						continue;
					int th = h * ( (n & 1) ? -1 : 1);
					int tk = k * ( (n & 2) ? -1 : 1);
					int tl = l * ( (n & 4) ? -1 : 1);
					if(calcHKL(crystal, &spot[spotn],th,tk,tl)){
						if(spotcount != 0){
							// Do something ?? 
							// To follow up!
						}	
						spotn++;
					}
				}
				
				if(abortcheck){
					if(*abort){
						//cerr << "lauegram::calc() : aborted" << endl;
						nspots = spotn;
						return false;
					}
				}
				
			}	
		}
	}
	
	if(debug)
		cerr << "lauegram::calc() : Calculated " << spotn << " spots" << endl;
	nspots = spotn;
	return true;
}

bool lauegram::isFundamental(int h, int k, int l){
	int maxdiv = abs(h);
	
	if( abs(k) > maxdiv )
		maxdiv = abs(k);
	if( abs(l) > maxdiv )
		maxdiv = abs(l);
	
	if(debug)
		cerr << "maxdiv = " << maxdiv << endl;
	
	for(int n = 2; n<=maxdiv ; n++){
		// Divide by n and see if we get integers!
		if((((double)h / n) == int((double)h / n)) && (((double)k / n) == int((double)k / n)) && (((double)l / n) == int((double)l / n))){
			if(debug)
				cerr << "Not a fundimental spot" << endl;
			return false;
		}
	}
	
	return true;
}
	
bool lauegram::calcHKL(Crystal *crystal, LaueSpot *spot, int h, int k, int l){
	Matrix G(3,1);
	Matrix R(3,1);
	Matrix kpri(3,1);
	double xx,yy;
	
	if(debug){
		cerr << "----------------------------------------" << endl;
		cerr << "HKL = " << h << "," << k << "," << l << endl;
	}	
	
	// Check if we have a fundimental spot or not
	
	if(!isFundamental(h,k,l))
		return false;
	
	R = Matrix(h,k,l);
	G = crystal->getUB() * R;
	double magG = G.mag();
	
	if(debug){
		cerr << "G:" << endl;
		cerr << G;
		cerr << "magG == " << magG << endl;
	}
	
	// Avoid spots along k_in
	
	if(magG < 0.0001){
		if(debug)
			cerr << "magG too small" << endl;
		//return 0;
	}
	
	Matrix k_in(1,0,0);
	
	double k_in_len = 0.5 * magG * magG / (k_in.dot(G));
	double maxdo;
	
	if(!calc_intensities)
		maxdo = 1;
	else
		maxdo = 2 * maxKvec * (k_in.dot(G)) / (magG * magG);
	
	if(debug){
		cerr << "maxdo = " << maxdo << endl;
		cerr << "k_in_len = " << k_in_len << endl;
	}
	
	if(abs(maxdo) < 1){
		if(debug)
			cerr << "k-vector too big (maxdo < 1)" << endl;
		return 0;
	}
	
	double cos_tth = 1.0-0.5*magG*magG/(k_in_len*k_in_len);
	
	if(debug)
		cerr << "cos_tth " << cos_tth << endl;
	if((cos_tth <= -1.00) || (cos_tth >= 1.00)) {
		if(debug)
			cerr << "cos_tth trap" << endl;
		return 0;
    }
	
	double tth = acos(fabs(cos_tth));
	
	if(debug)
		cerr << "calculated tth = " << 180 - (180.0 * tth / M_PI) << endl;	
	if((cos_tth >= 0.00)) {
		if(debug)
			cerr << "kpri pointing wrong direction" << endl;
		return 0;
	}
		
	kpri = (G * (1/k_in_len)) - k_in;
	
	// Calculate intensity 
	
	double inten = 0;
	int ndocalc = 1;
	
	
	for(int n = 1; n <= abs((int)maxdo) ; n++){
		int hh = h * n;
		int kk = k * n;
		int ll = l * n;
		double this_k_in_len = n * k_in_len;
		
		if((this_k_in_len < minKvec) && calc_intensities){
			if(debug)
				cerr << "k-vector too small" << endl;
			continue;
		}
		
		double sinthoverlambda = /*2.0 * M_PI*/ sin((M_PI - tth) / 2.0) * (this_k_in_len);
		
		if(debug)
			cerr << "sin(th) / lambda = " << sinthoverlambda << endl;
		
		complex<double> sfactor = crystal->calcXSFactor(hh,kk,ll, sinthoverlambda, 
													   1.0 / this_k_in_len);
		if(debug)
			cerr << "f = " << sfactor << endl;
		double thisinten = real(conj(sfactor) * sfactor);
		if(intensityCorrection() & IntensitySelfAbsorption)
			thisinten = thisinten * calcAbs( crystal, 1.0 / this_k_in_len , (M_PI - tth) / 2);
		if(intensityCorrection() & IntensityXRayTube)
			thisinten = thisinten * tubeIntensity( 1.0 / this_k_in_len , HC_OVER_E / minKvec);
		thisinten = thisinten * calcIntensityCorrection( 1.0 / this_k_in_len, M_PI - tth);	
		inten = inten + thisinten;
		
		if(XRay::tubeCharacteristicIntensity( tube_z, 1.0 / this_k_in_len ) > 0.25){
			spot->setCharacteristic(true);
		}
		
		if(debug)
			cerr << "Calculated (" << hh << "," << kk << "," << ll << ")" << endl;
		
		ndocalc++;
	}
	inten = inten / ndocalc;

	if(inten < inten_thresh){
		if(debug){
			cerr << "Spot Extinct (" 
			<< h << "," << k << "," << l << ")" << endl;
		}
		return 0;
	}
	
	// Make spot
	
	xx = kpri.dot(Matrix(0,-1 * fsDistance,0)) * (1/fabs(cos_tth));
	yy = kpri.dot(Matrix(0,0,fsDistance)) * (1/fabs(cos_tth));
	
	// Set the LaueSpot class variable to update the values
	
	spot->setXYIN(xx,yy,inten,abs((int)maxdo));
	spot->setHKL(h,k,l);
	return 1;
	
}

double lauegram::calcAbs(Crystal* crystal, double lambda, double th){
	double mu = crystal->xRayAbsorptionCoeff(lambda);
	double a;
	if(mu != 0){
		a = cos((M_PI / 2) - th) + (cos((M_PI / 2) - th) / cos(2*((M_PI / 2) - th)));
		a = 1000.0 / (a * mu);
	} else {
		a = 1.0;
	}
	
	//cerr << "lambda = " << lambda << " " << "calcAbs = " << a << endl;
	return a;
}

void lauegram::calcInten(double minx, double maxx, double miny, double maxy){
	double maxinten = maxSpotInten(minx, maxx, miny, maxy);
	for(int i = 0; i < nspots; i++){
		double inten = spot[i].I() / (maxinten * detsaturation);
		if(inten < detlevel){
			spot[i].setDetI(0.0);
			continue;
		}
		if(inten > 1){
			spot[i].setDetI(1.0);
			continue;
		}
		spot[i].setDetI(inten);
	}
}

int lauegram::findSpot(double xx, double yy, double acc){
	for(int i = 0; i < nspots; i++){
		double x = spot[i].X();
		double y = spot[i].Y();
		if((x > (xx - acc)) && (x < (xx + acc)))
			if((y > (yy - acc)) && (y < (yy + acc)))
				// We have found the spot!
				return i;
	}
	
	// If we are here then we have not found a spot
	return -1;
}

// **********************************************************************************

// Helper routines

Matrix lauegram::filmposToVector(double x, double y){
	Matrix kpri(fsDistance, -1.0 * x, y);
	Matrix k(1,0,0);
	Matrix G(1,0,0);
	
	kpri = kpri / kpri.mag();
	
	G = k + kpri;
	G = G / G.mag();
	return G;
}

// **********************************************************************************

// Indexing routines (opperate on the lauegram class)

int lauegram::indexLaue(Crystal *crystal, int numIndexSpots, double indexSpots[100][2], int *nOrientations, LaueOrientations *orientations) {
	if(numIndexSpots < 2)
		return false;

	*nOrientations = 0;

	Matrix primary(fsDistance,-1.0 * indexSpots[0][0] , indexSpots[0][1]);
	Matrix secondary(fsDistance, -1.0 * indexSpots[1][0] , indexSpots[1][1]);
	Matrix k_in(1,0,0);
	
	primary = k_in + (primary / primary.mag());
	secondary = k_in + (secondary / secondary.mag());

	double theta = cos(primary.dot(secondary) / (primary.mag() * secondary.mag()));

	int maxH = indexingMaxH; int maxK = indexingMaxK ; int maxL = indexingMaxL;

	Crystal newOrient = *crystal;
	newOrient.setFreeRotate(true);
	
	int oldCalcIntensities = calcIntensities();
	setCalcIntensities(false);
	
	for(int h1 = (-1 * maxH) ; h1 <= maxH ; h1++){
		for(int k1 = (-1 * maxK) ; k1 <= maxK ; k1++){
			for(int l1 = (-1 * maxL) ; l1 <= maxL ; l1++){
				for(int h2 = (-1 * maxH) ; h2 <= maxH ; h2++){
					for(int k2 = (-1 * maxK) ; k2 <= maxK ; k2++){
						for(int l2 = (-1 * maxL) ; l2 <= maxL ; l2++){
							if(isFundamental(h1,k1,l1)){
								if(isFundamental(h2,k2,l2)){
									// Check the angle between vectors
									Matrix vec1(h1,k1,l1);
									Matrix vec2(h2,k2,l2);
									
									vec1 = crystal->getB() * vec1;
									vec1 = vec1 / vec1.mag();
									vec2 = crystal->getB() * vec2;
									vec2 = vec2 / vec2.mag();
									
									double gamma = cos(vec1.dot(vec2));
									
									if(abs(gamma - theta) < indexingAngularTol){
										// Ok so we have a match to primary and secondary
										// try all other reflections then
										//cerr << "MATCH : (" << h1 << "," << k1 << "," << l1 << ") and (";
										//cerr << h2 << "," << k2 << "," << l2 << ")" << endl;
										// Now calculate a possible U matrix from the Primary and Secondary
										
										Matrix Tc(3,3), Td(3,3), newU(3,3);
										Matrix Tc2(3,1), Tc3(3,1);
										Matrix Td2(3,1), Td3(3,1);
										
										Tc3 = vec1 ^ vec2;
										Tc3 = Tc3 / Tc3.mag();
										Tc2 = Tc3 ^ vec1;
										Tc2 = Tc2 / Tc2.mag();
										
										primary = primary / primary.mag();
										secondary = secondary / secondary.mag();
										
										Td3 = primary ^ secondary;
										Td3 = Td3 / Td3.mag();
										Td2 = Td3 ^ primary;
										Td2 = Td2 / Td2.mag();
										
										for(int i = 0 ; i < 3 ; i++ ){
											// Load up the matricies Tc and Td;
											Tc.Set(i,0,vec1.Get(i,0));
											Tc.Set(i,1,Tc2.Get(i,0));
											Tc.Set(i,2,Tc3.Get(i,0));
											
											Td.Set(i,0,primary.Get(i,0));
											Td.Set(i,1,Td2.Get(i,0));
											Td.Set(i,2,Td3.Get(i,0));
										}
										
										newU = Td * Tc.transpose();
										newOrient.setU(newU);
										
										// Now calculate spot position for secondary
										
										LaueSpot s;
										if(calcHKL(&newOrient, &s, h2,k2,l2)) {
											// Check if this spot is in the right place.
											
											double diff = pow(pow(s.X() - indexSpots[1][0],2) + pow(s.Y() - indexSpots[1][1],2),0.5);
											
											if(abs(diff) < indexingSpotTol){
												bool abort = false;
												int spotcount = 0;
												calc(&newOrient, &abort, &spotcount);
										
												int nFoundSpots = 2;
												double error = 0;
										
												for(int n = 2; n < numIndexSpots ; n++){
													// Check if there is a spot in this place
													int sn = findSpot(indexSpots[n][0], indexSpots[n][1], indexingSpotTol);
													if( sn != -1){
														nFoundSpots++;
														error = error + pow(pow(spot[sn].X() - indexSpots[n][0],2) + pow(spot[sn].Y() - indexSpots[n][1],2),0.5);
													}
												}
												
												// Now store the solution
												//cerr << "Orientations = " << (*nOrientations) << endl;
												orientations[*nOrientations].setU(newU);
												orientations[*nOrientations].setNumMatches(nFoundSpots);							
												orientations[*nOrientations].setPatternError(error / nFoundSpots);
												orientations[*nOrientations].setP(h1,k1,l1);
												orientations[*nOrientations].setS(h2,k2,l2);
												(*nOrientations)++;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	sort(orientations, orientations + (*nOrientations));
	
	// reset calcinten
	
	setCalcIntensities(oldCalcIntensities);
	
	// Recalculate on origional crystal
	
	bool abort = false; int spotcount = 0;
	calc(crystal, &abort, &spotcount);
	
	return true;
}

bool operator<(const LaueOrientations& a, const LaueOrientations& b){
	// a < b
	if(a.getNumMatches() == b.getNumMatches())
		return a.getPatternError() < b.getPatternError();
	else
		return a.getNumMatches() > b.getNumMatches();
}

// **********************************************************************************

// Re-orientation routines (class = Reorientation)

Reorientation::Reorientation():
primary(3,1), secondary(3,1), UB(3,3)
{ 
	mode = Unknown;
}

int Reorientation::getMode(void){
	return mode;
}

void Reorientation::setPrimary(const Matrix p){
	primary = p;
}

void Reorientation::setSecondary(const Matrix s){
	secondary = s;
}

void Reorientation::setPrimary(double a, double b, double c){
	primary = Matrix(a,b,c);
}

void Reorientation::setSecondary(double a, double b, double c){
	secondary = Matrix(a,b,c);
}

void Reorientation::calc(void){
	calc(XAxis);
}

void Reorientation::calc(int thismode){
	Matrix hPhi(3,1);
	Matrix hPhi2(3,1);
	Matrix hPhi3(3,1);
	Matrix R(3,3);
	
	mode = thismode;
	
	hPhi = UB * primary;
	
	angles[0][0] = atan2(hPhi.Get(1,0),hPhi.Get(2,0));
	angles[0][1] = atan2(-1.0 * sqrt(pow(hPhi.Get(2,0),2) + pow(hPhi.Get(1,0),2)), hPhi.Get(0,0));
	angles[0][2] = 0.0;

	angles[1][0] = atan2(hPhi.Get(2,0),-1.0 * hPhi.Get(1,0));
	angles[1][1] = 0.0;
	angles[1][2] = atan2(-1.0 * sqrt(pow(hPhi.Get(2,0),2) + pow(hPhi.Get(1,0),2)),hPhi.Get(0,0));
	
	angles[2][0] = 0.0;
	angles[2][1] = atan2(-1.0 * hPhi.Get(2,0),hPhi.Get(0,0));
	angles[2][2] = atan2(hPhi.Get(1,0), sqrt(pow(hPhi.Get(0,0),2) + pow(hPhi.Get(2,0),2)));
	
	if(mode == Zone){
		hPhi2 = UB * secondary;
	
		angles[3][0] = atan2(hPhi2.Get(1,0),hPhi2.Get(2,0));
	
		R = R.rotateX(angles[3][0]);
		cerr << "Rotate X " << endl << R << endl;
		hPhi3 = R * hPhi;
		cerr << "hphi3 = " << endl << hPhi3 << endl;
		
		angles[3][1] = atan2(-1.0 * hPhi3.Get(2,0),hPhi3.Get(0,0));
		angles[3][2] = atan2(hPhi3.Get(1,0), sqrt(pow(hPhi3.Get(0,0),2) + pow(hPhi3.Get(2,0),2)));
	} else {
		angles[3][0] = 0.0;
		angles[3][1] = 0.0;
		angles[3][2] = 0.0;
	}
}

void Reorientation::setUB(const Matrix newUB){
	UB = newUB;
}

double* Reorientation::getAngles(int n){
	return angles[n];
}

Matrix Reorientation::getPrimary(void){
	return primary;
}

Matrix Reorientation::getSecondary(void){
	return secondary;
}

// **********************************************************************************

// LaueReorientation Routines

