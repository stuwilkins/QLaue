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

#ifndef SBWMATRIX
#define SBWMATRIX 1
//#include <math>
#include <iostream>
#include <complex>

using namespace std;

//typedef double dcplx;

class Matrix {
private:
  int rows;
  int cols;
  double *data;
 public:
  double _get(int i) const ;
  void _put(int i, double num);
  Matrix(int c,int r);
  Matrix(double a, double b, double c);
  Matrix(double a, double b, double c,
		 double d, double e, double f,
		 double g, double h, double i);	  
  Matrix(double a, double b, double c, double d, 
		 double e, double f, double g, double h, 
		 double i, double j, double k, double l);
  void init(int c, int r);
  Matrix(void);
  Matrix(const Matrix &M);
  ~Matrix();
  void dump();
  int Rows() const;
  int Cols() const;
  int Size() const;
  double Get(int i,int j) const;
  Matrix Get(void);
  void Set(int i, int j, double m);
  void identity(void);
  void zero(void);
  
  // Matrix operatons.
  
  double dot(Matrix other);
  double mag(void);
  Matrix transpose(void);
  double det(void);
  Matrix inv(void);
  
  // Rotations of matrix.
  
  Matrix rotateX(double alpha);
  Matrix rotateY(double alpha);
  Matrix rotateZ(double alpha);
  
  // Overloaded operators 
  
  void operator+=(Matrix M);
  void operator*=(double c);
  Matrix operator=(Matrix M);
  friend Matrix operator*(Matrix a,Matrix b);
  friend Matrix operator*(Matrix a,double b);
  friend Matrix operator/(Matrix a,Matrix b);
  friend Matrix operator/(Matrix a,double b);
  friend Matrix operator+(Matrix a,Matrix b);
  friend Matrix operator-(Matrix a,Matrix b);
};



Matrix operator^(Matrix a,Matrix b); // This is used for cross product
ostream& operator<<(ostream& out,Matrix matrix);
#endif
