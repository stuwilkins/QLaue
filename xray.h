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

#ifndef _XRAY_H
#define _XRAY_H 1

#include <complex>
using std::complex;

#define HC_OVER_E 12.39842
#define R_ZERO 2.8719e-15 // electron radius (meters)
#define N_A 6.0221415e23 // Avogadro's number

typedef struct {
	double K;
	double L[3];
	double M[5];
	double N[7];
	//double O[5];
	//double P[3];
} XrayBinding;

typedef struct {
	int Z;
	double data[2000][3];
	int nvalues;
} Xrayf1f2;

typedef struct {
	double K[3];
	double L[5];
} XrayEmission;

class XRay {
private:	
	double l;
	static const double xrayatomic_mass[];
	static const XrayBinding xraybe[];
	static const XrayEmission xrayel[];
	static const Xrayf1f2 xrayf1f2[];
	static const double xrayf0_c[];
	static const double xrayf0_a[][5];
	static const double xrayf0_b[][5];
	static const char   xraye[][3];
	
	// store the intensity values;
	
	int inten_corr;
	int poln;
public:
	XRay();	
		
	double	lambda(void) const				{ return l; }
	double	energy(void) const				{ return HC_OVER_E / l; }
	void	setLambda(double newl)			{ l = newl; }
	void	setEnergy(double newe)			{ l = HC_OVER_E / newe; }
	
	void	setIntensityCorrection(int i)	{ inten_corr = i; }
	int		intensityCorrection(void)		{ return inten_corr; }
	
	void	setIncidentPoln(int i)			{ poln = i; }
	int		incidentPoln(void)				{ return poln; }
	double  calcIntensityCorrection(double lambda, double tth);
	
	static double getXf0(int Z, double sinth_over_lambda);
	static complex<double> getXf1f2(int Z, double lambda);
	static const char* elementName(int Z)  { return xraye[Z-1]; }
	static int elementNumber(const char* name);
	static XrayBinding bindingEnergy(int Z) { return xraybe[Z]; }
	static double massAbsorption(int Z, double lambda);
	static double tubeIntensity(double lambda, double V);
	static double tubeCharacteristicIntensity(int tubeZ, double lambda);
	
	enum {
		IntensityLorentz = 1,
		IntensityPolarization = 2
	};
	enum {
		PolnSigma = 1,
		PolnPi = 2,
		PolnUnpol = 3
	};
};

#endif