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

#include <complex>
#include <iostream>
#include <string.h>
using std::complex;
using namespace std;
#include "xray.h"
#include "xraybe.h"
#include "xrayel.h"
#include "xrayam.h"
#include "sfact_f0.h"
#include "sfact_f1f2.h"

// Calculate xf0 from tables hard coded in atom.h
// f0[k] = c + [SUM a_i*EXP(-b_i*(k^2)) ]				    
//             i=1,5	

XRay::XRay(){
	poln = PolnSigma;
}

double XRay::getXf0(int Z, double sinth_over_lambda){
	double xf0;
	
	xf0 = xrayf0_c[Z-1];
	
	for(int i=0;i<5;i++){
		xf0 = xf0 + xrayf0_a[Z-1][i]*exp(-1.0 * xrayf0_b[Z-1][i] * sinth_over_lambda);
	}
	return xf0;
}

complex<double> XRay::getXf1f2(int Z, double lambda){
	complex<double> thisf1f2(Z,0);
	double ene = HC_OVER_E * 1000 / lambda; 
	if(Z > 92) 
		return thisf1f2;
	for(int i=1;i<xrayf1f2[Z-1].nvalues;i++){
		if(xrayf1f2[Z-1].data[i][0] > ene){
			// Found value so do a linear interpolation
			// With last value
			double estep = xrayf1f2[Z-1].data[i][0] - xrayf1f2[Z-1].data[i-1][0];
			double efrac = (ene - xrayf1f2[Z-1].data[i-1][0]) / estep;
			double f1step = xrayf1f2[Z-1].data[i][1] - xrayf1f2[Z-1].data[i-1][1];
			double f2step = xrayf1f2[Z-1].data[i][2] - xrayf1f2[Z-1].data[i-1][2];
			thisf1f2 = complex<double>(xrayf1f2[Z-1].data[i-1][1] + (f1step * efrac),
									   -1.0 * xrayf1f2[Z-1].data[i-1][2] + (f2step * efrac));
			break;
		}
	}
	
	return thisf1f2;
}

double XRay::massAbsorption(int Z, double lambda) {
	double massAbs;
	massAbs = 2 * lambda * 1e-10 * R_ZERO * N_A * 1e3 / xrayatomic_mass[Z-1];
	complex<double> f1f2 = getXf1f2(Z,lambda);
	massAbs = massAbs * f1f2.imag();
	return massAbs;
}

double XRay::tubeIntensity(double lambda, double V){
	double e = HC_OVER_E / lambda;
	
	double i = (V - e) / e; // this is the intensity
	if(i < 0)
		return 0.0;
	
	double mu =  massAbsorption(4,lambda) * 1848; // Density of Be
	if(mu >= 0){
		i = 0;
	} else {
		i = i * exp(0.0005 * mu); // 0.5mm Be Window
	}
	return i;
}

double XRay::tubeCharacteristicIntensity(int tubeZ, double lambda){
	double e = HC_OVER_E / lambda;
	// Try adding lines for K-alpha and K-beta
	double i = 0;
	for(int n=0 ; n < 3; n++){
		i = i + (pow(0.001,2) / (pow(e - xrayel[tubeZ - 1].K[n],2) + pow(0.001, 2)));
	}
	return i;
}

double XRay::calcIntensityCorrection(double lambda, double tth){
	double icorr = 1;
	
	if(inten_corr & IntensityLorentz)
		if((tth / 2.0) != 0)
			icorr = icorr * pow(lambda,3) / pow(sin(tth / 2.0),2);
	if(inten_corr & IntensityPolarization){
		if(poln == PolnSigma){
			// Do nothing !
		} else if(poln == PolnPi) {
			icorr = icorr * ( pow(cos(tth),2) );
		} else {
			icorr = icorr * ( 1 + pow(cos(tth),2) );
		}
	}
	return icorr;
}

int XRay::elementNumber(const char* name){
	for(int i=0;i<95;i++){
		if(!strcmp(name, xraye[i]))
			return i + 1;
	}
	return -1;
}
