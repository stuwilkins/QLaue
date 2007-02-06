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

#ifndef _LAUE_H
#define _LAUE_H

#include "matrix.h"
#include "atom.h"
#include "crystal.h"
#include "xray.h"

// Class for possible orientations

class LaueOrientations {
private:
	Matrix U;
	Matrix P, S;
	int numMatches;
	double patternError;
public:
	// Constructors
	
	LaueOrientations();
	LaueOrientations(const LaueOrientations& L);
	
	void setU(Matrix newU)							{ U = newU; }
	void setP(int a, int b, int c)					{ P.Set(0,0,a); P.Set(1,0,b); P.Set(2,0,c);}
	void setS(int a, int b, int c)					{ S.Set(0,0,a); S.Set(1,0,b); S.Set(2,0,c);}
	void setNumMatches(int n)						{ numMatches = n; }
	void setPatternError(double e)					{ patternError = e; }
	
	Matrix getU(void) const							{ return U; }
	Matrix getS(void) const							{ return S; }
	Matrix getP(void) const							{ return P; }
	int getNumMatches(void) const					{ return numMatches; }
	double getPatternError(void) const				{ return patternError; }
};

bool operator<(const LaueOrientations& a, const LaueOrientations& b);

class Reorientation {
private:
	double angles[4][3];
	Matrix primary;
	Matrix secondary;
	Matrix UB;
	int mode;
public:
	// Constructors
		
	Reorientation();
	
	// Calculation
	void calc(void);
	void calc(int mode);
	
	// get Parameters
	
	double* getAngles(int n);
	Matrix getPrimary(void);
	Matrix getSecondary(void);
	int getMode(void);
	
	// Set parameters
	
	void setPrimary(const Matrix p);
	void setPrimary(double a, double b, double c);
	void setSecondary(const Matrix s);
	void setSecondary(double a, double b, double c);
	void setUB(const Matrix newUB);
	
	enum {
		Unknown = 0,
		XAxis = 1,
		Zone = 2
	};
	enum {
		RotateXY = 0,
		RotateXZ = 1,
		RotateYZ = 2,
		RotateXYZ = 3
	};
};

class LaueSpot {
private:
	double x, y;
	double polarX, polarY;
	double h, k, l;
	double theta, phi;
	double inten;
	double detInten;
	int multiplicity;
	int fromCharateristic;
public:
	LaueSpot(void);
	double X(void);
	double Y(void);
	double I(void);
	double detI(void);
	double H(void);
	double K(void);
	double L(void);
	double polarPhi(void)					{ return phi; }
	double polarTheta(void)					{ return theta; }
	int getMaxdo(void);
	void setXYIN(double xx, double yy, double ii, int n);
	void setHKL(double hh, double kk, double ll);
	void setPolar(double t, double p)		{ phi = p; theta = t; }
	void setPolarXY(double xx, double yy)	{ polarX = xx; polarY =yy; }
	void setI(double i);
	void setDetI(double i);
	void setCharacteristic(bool yesno)		{ fromCharateristic = yesno; }
	bool characteristic(void)				{ return fromCharateristic; }
};

class lauegram : public XRay {
	
private:
	int nspots;
	int debug;
	double fsDistance;
	double inten_thresh;
	double maxKvec;
	double minKvec;
	double detlevel;
	double detsaturation;
	int calc_precision;
	bool calc_intensities;
	int tube_z;
	LaueSpot *spot;
	int indexingMaxH;
	int indexingMaxK;
	int indexingMaxL;
	double indexingAngularTol;
	double indexingSpotTol;
	
public:
	// Constructors
	lauegram();
	lauegram(const lauegram& L);
	~lauegram();
	void init(void);
	
	// Functions
	
	int getIndexingMaxH(void)				{ return indexingMaxH; }
	int getIndexingMaxK(void)				{ return indexingMaxK; }
	int getIndexingMaxL(void)				{ return indexingMaxL; }
	void setIndexingMaxH(int x)				{ indexingMaxH = x; }
	void setIndexingMaxK(int x)				{ indexingMaxK = x; }
	void setIndexingMaxL(int x)				{ indexingMaxL = x; }
	double getIndexingAngularTol(void)		{ return indexingAngularTol; }
	double getIndexingSpotTol(void)			{ return indexingSpotTol; }
	void setIndexingSpotTol(double x)		{ indexingSpotTol = x; }
	void setIndexingAngularTol(double x)	{ indexingAngularTol = x; }
	
	// Functions
	
	bool isFundamental(int h, int k, int l);
	double detSaturation(void);
	double detLevel(void);
	void setDet(double dl, double ds);
	void setDetLevel(double dl);
	double getDetLevel(void);
	void setDetSatLevel(double ds);
	double getDetSatLevel(void);
	void setDebug(int value);
	void setDistance(double d);
	
	void setLambdaRange(double min, double max);
	double getLambdaRangeMax(void);
	double getLambdaRangeMin(void);
	bool calc(Crystal *crystal, bool* abort = 0, int *spotcount = 0);
	bool calcHKL(Crystal *crystal, LaueSpot *spot, int h, int k, int l);
	void calcInten(double minx, double maxx, double miny, double maxy);
	
	double getDistance(void);
	int getNspots(void);
	void setNspots(int n)				{ nspots = n; }
	double maxSpotInten(void);
	double maxSpotInten(double minx, double maxx, double miny, double maxy);
	
	double getMaxKVec(void)				{ return maxKvec;}
	
	int calcPrecision(void)				{ return calc_precision; }
	void setCalcPrecision(int p)		{ calc_precision = p; }
	
	int tubeZ(void)						{ return tube_z; }
	void setTubeZ(int Z)				{ tube_z = Z; }
	
	LaueSpot* getSpot(int n)				{ return &spot[n]; }
	int findSpot(double xx, double yy, double acc);
	double calcAbs(Crystal* crystal, double lambda, double th);
	
	void setCalcIntensities(bool y)		{ calc_intensities = y; }
	bool calcIntensities(void)			{ return calc_intensities; }
	
	// Helper routines
	
	Matrix filmposToVector(double x, double y);
	
	// Indexing routines
	
	int indexLaue(Crystal *crystal, int numIndexSpots, double indexSpots[100][2], int *nOrientations, LaueOrientations *orientations);
		
	// operators
	
	lauegram& operator=(const lauegram &L);
	
	// Other classes
	
	Reorientation reorientation;
	
	enum {
		IntensityXRayTube = 256,
		IntensitySelfAbsorption = 512
	};
	
};

#endif