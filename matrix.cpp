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

#include <iostream>
#include <complex>
#include "matrix.h"

using namespace std;

void Matrix::init(int r,int c){
  rows = r ; cols = c;
  data = new double[rows * cols];
  for(int i = 0; i < (rows * cols) ; i++ )
	  data[i] = 0.0;
  //cerr << data;
  //cerr << "Created Matrix with " << rows << " and " << cols <<"\n";
}

Matrix::Matrix(int r, int c){
  //cerr << "creating empty " << r << " x " << c << " matrix" << endl;
  init(r,c);
}

Matrix::Matrix(void): data(NULL) {
	cerr << "WARNING :: creating matrix out of thin air\n";
	data = NULL;
}

Matrix::Matrix(double a, double b, double c,
			   double d, double e, double f,
			   double g, double h, double i){
	// Initilize a 3x3 matrix
	init(3,3);
	Set(0,0,a);
	Set(0,1,b);
	Set(0,2,c);
	
	Set(1,0,d);
	Set(1,1,e);
	Set(1,2,f);
	
	Set(2,0,g);
	Set(2,1,h);
	Set(2,2,i);
}

Matrix::Matrix(double a, double b, double c, double d, 
			   double e, double f, double g, double h, 
			   double i, double j, double k, double l){
	// Initilize a 3x4 matrix
	init(3,4);
	Set(0,0,a);
	Set(0,1,b);
	Set(0,2,c);
	Set(0,3,d);
	
	Set(1,0,e);
	Set(1,1,f);
	Set(1,2,g);
	Set(1,3,h);
	
	Set(2,0,i);
	Set(2,1,j);
	Set(2,2,k);
	Set(2,3,l);
	
	//cerr << *this;
}

Matrix::Matrix(double a, double b, double c){
	// Initilize a 3x1 matrix
	init(3,1);
	Set(0,0,a);
	Set(1,0,b);
	Set(2,0,c);
}

Matrix::Matrix(const Matrix &M) {
	rows = M.rows;
	cols = M.cols;
	data = new double[cols*rows];
	for(int i=0; i < cols*rows; i++) {
		data[i] = M.data[i];
	}
}

Matrix::~Matrix(){
	//cerr << "Deleteing :" << data << "\n";
	if(data != NULL){
		delete data;
	} else {
		cerr << "Deleteing NULL data" << endl;
	}
}

double Matrix::dot(Matrix other) {
	double dot = 0;
	for(int i = 0 ; i < 3 ; i ++ ){
		dot = dot + (Get(i,0) * other.Get(i,0));
	}
	return dot;
}

int Matrix::Rows() const {
  return rows;
}

int Matrix::Cols() const {
  return cols;
}

int Matrix::Size() const {
  return rows*cols;
}

void Matrix::Set(int j, int i, double m){
  data[(j*cols) + i] = m;
}

double Matrix::Get(int j, int i) const{
  return data[(j * cols) + i];
}

double Matrix::_get(int i) const {
  return data[i];
}

void Matrix::_put(int i, double num){
  data[i] = num;
}

Matrix Matrix::transpose(void){
	// Transpose of the matrix .. 
	Matrix dest(cols,rows);
	for(int i = 0 ; i < cols ; i++ )
		for( int j = 0 ; j < rows ; j++ )
			dest.Set(j,i,Get(i,j));
	return dest;
}

Matrix Matrix::operator=(const Matrix M){
	cols = M.cols;
	rows = M.rows;
	for(int i=0;i<(cols*rows);i++){
		data[i] = M.data[i];
	}
	return *this;
}

void Matrix::operator+=(const Matrix M) {
  int i;  
  for(i=0;i<(cols*rows);i++){
    data[i] += M._get(i);
  }
}

void Matrix::operator*=(double c){
  int i;
  for(i=0;i<(cols*rows);i++){
    data[i] *= c;
  }
}

Matrix operator+(Matrix a,Matrix b){
	Matrix result(a.Rows(),a.Cols());
	for(int i = 0 ; i < a.Rows() ; i++)
		for( int j = 0 ; j < a.Cols() ; j++)
			result.Set(j,i,a.Get(j,i) + b.Get(j,i));
	return result;
}

Matrix operator-(Matrix a,Matrix b){
	Matrix result(a.Rows(),a.Cols());
	for(int i = 0 ; i < a.Rows() ; i++)
		for( int j = 0 ; j < a.Cols() ; j++)
			result.Set(j,i,a.Get(j,i) - b.Get(j,i));
	return result;
}

Matrix operator^(Matrix a,Matrix b){
	Matrix result(3,1);
	result.Set(0,0,( a.Get(1,0) * b.Get(2,0) ) - ( a.Get(2,0) * b.Get(1,0) ));
	result.Set(1,0,( a.Get(2,0) * b.Get(0,0) ) - ( a.Get(0,0) * b.Get(2,0) ));
	result.Set(2,0,( a.Get(0,0) * b.Get(1,0) ) - ( a.Get(1,0) * b.Get(0,0) ));
	return result;
}

Matrix operator/(Matrix a,double b){
	Matrix result(a.Rows(),a.Cols());
	for( int i = 0 ; i < a.Rows() ; i++ ){
		for( int j = 0 ; j < a.Cols() ; j++ ){
			result.Set(j,i,a.Get(j,i) / b);
		}
	}
	return result;
}

Matrix operator*(Matrix a,double b){
	Matrix result(a.Rows(),a.Cols());
	for( int i = 0 ; i < a.Rows() ; i++ ){
		for( int j = 0 ; j < a.Cols() ; j++ ){
			result.Set(j,i,a.Get(j,i) * b);
		}
	}
	return result;
}

Matrix operator*(Matrix a,Matrix b){
	int i,j,k;
	Matrix result(a.Rows(),b.Cols());

	if(a.Cols() != b.Rows())
		cerr << "WARNING : matrix inner dimen. not equal\n";
  
	for(i=0;i<a.Rows();i++){
		for(k=0;k<b.Cols();k++){
			result.Set(i,k,0);
			for(j=0;j<a.Cols();j++){
				result.Set(i,k,(a.Get(i,j) * b.Get(j,k)) + result.Get(i,k));
			}
		}
	}
	return result;
}

void Matrix::dump(){
  int i,j,n=0;

  cerr << "Matrix : "<< rows << "x" << cols << "\n";
  
  for(i=0;i<rows;i++){
    for(j=0;j<cols;j++){
      cerr << data[n++];
    }
    cerr << "\n";
  }
}

void Matrix::zero(void) {
	for(int i = 0 ; i < (cols*rows) ; i++ )
		data[i] = 0;
}

void Matrix::identity(void) {
  int i,j,c = 0;
  double n = 1;
  for(i=0;i<rows;i++){
    for(j=0;j<cols;j++){
      if(i==j) {
	data[c++]= n;
      } else {
	data[c++] = 0;
      }
    }
  }
  
}

double Matrix::mag(void) {
	return pow(pow(data[0],2) + pow(data[1],2) + pow(data[2],2),0.5);
}

ostream& operator<<(ostream& out,Matrix matrix){
  int i,j;
  string output;

  for(i=0;i<matrix.Rows();i++){
	  out << "( ";
	  for(j=0;j<matrix.Cols();j++){
		  out << matrix.Get(i,j) << " ";
	  }
	  out << ")" << endl;
  }
  return out;
}

Matrix Matrix::rotateX(double alpha) {
	// Taken from MathWorld
	// http://mathworld.wolfram.com/RotationMatrix.html
	
	Matrix R(3,3);
	
	R.Set(0,0,1);
	R.Set(0,1,0);
	R.Set(0,2,0);
	
	R.Set(1,0,0);
	R.Set(1,1,cos(alpha));
	R.Set(1,2,sin(alpha));
	
	R.Set(2,0,0);
	R.Set(2,1,-1.0 * sin(alpha));
	R.Set(2,2,cos(alpha));
	
	return R;
}

Matrix Matrix::rotateY(double alpha) {
	// Taken from MathWorld
	// http://mathworld.wolfram.com/RotationMatrix.html
	// Rotates the matrix by the Euler angles
	
	Matrix R(3,3);
	
	R.Set(0,0,cos(alpha));
	R.Set(0,1,0);
	R.Set(0,2,-1.0*sin(alpha));
	
	R.Set(1,0,0);
	R.Set(1,1,1);
	R.Set(1,2,0);
	
	R.Set(2,0,sin(alpha));
	R.Set(2,1,0);
	R.Set(2,2,cos(alpha));
	
	return R;
}

Matrix Matrix::rotateZ(double alpha) {
	// Taken from MathWorld
	// http://mathworld.wolfram.com/RotationMatrix.html
	// Rotates the matrix by the Euler angles
	
	Matrix R(3,3);
		
	R.Set(0,0,cos(alpha));
	R.Set(0,1,sin(alpha));
	R.Set(0,2,0);
	
	R.Set(1,0,-1.0*sin(alpha));
	R.Set(1,1,cos(alpha));
	R.Set(1,2,0);
	
	R.Set(2,0,0);
	R.Set(2,1,0);
	R.Set(2,2,1);
	
	return R;
}

double Matrix::det(void) {
	// Calculate the determinant of a 3x3 matrix
	double det = 0;
	
	det = Get(0,0) * Get(1,1) * Get(2,2);
	det-= Get(0,0) * Get(1,2) * Get(2,2);
	det-= Get(0,2) * Get(1,0) * Get(2,2);
	det+= Get(0,1) * Get(1,2) * Get(2,0);
	det+= Get(0,2) * Get(1,0) * Get(2,1);
	det-= Get(0,2) * Get(1,1) * Get(2,0);

	return det;
}

Matrix Matrix::inv(void) {
	// Calculate the inverse of a 3x3 matrix
	Matrix inverse(3,3);
	
	inverse.Set(0,0,(Get(1,1)*Get(2,2)) - (Get(1,2)*Get(2,1)));
	inverse.Set(0,1,(Get(0,2)*Get(2,1)) - (Get(0,1)*Get(2,2)));
	inverse.Set(0,2,(Get(0,1)*Get(1,2)) - (Get(0,2)*Get(1,1)));
	
	inverse.Set(1,0,(Get(1,2)*Get(2,0)) - (Get(1,0)*Get(2,2)));
	inverse.Set(1,1,(Get(0,0)*Get(2,2)) - (Get(0,2)*Get(2,0)));
	inverse.Set(1,2,(Get(0,2)*Get(1,0)) - (Get(0,0)*Get(1,2)));
	
	inverse.Set(2,0,(Get(1,0)*Get(2,1)) - (Get(1,1)*Get(2,0)));
	inverse.Set(2,1,(Get(0,1)*Get(3,0)) - (Get(0,0)*Get(2,1)));
	inverse.Set(2,2,(Get(0,0)*Get(1,1)) - (Get(0,1)*Get(1,0)));
	
	inverse = inverse * (1.0 / (*this).det());
	
	//cerr << "Inverse" << endl << inverse << endl; 
	
	return inverse;
}