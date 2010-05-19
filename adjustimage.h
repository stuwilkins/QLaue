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
 
 $Revision: 18 $
 $Author: stuwilkins $
 $Date: 2010-05-05 13:29:55 -0400 (Wed, 05 May 2010) $
 $HeadURL: https://qlaue.svn.sourceforge.net/svnroot/qlaue/trunk/lauewidget.cpp $
 
*/

#ifndef _ADJUSTIMAGE_H
#define _ADJUSTIMAGE_H 1

template<class T> inline const T& kClamp( const T& x, const T& low, const T& high );

inline int changeBrightness( int value, int brightness );
inline int changeContrast( int value, int contrast );
inline int changeGamma( int value, int gamma );
int changeMin( int value, int min);
int changeMax( int value, int max);

inline int changeUsingTable( int value, const int table[] );

template< int operation( int, int ) > static QImage changeImage( const QImage& image, int value );
QImage convertToPseudoColor(const QImage& image);

QImage changeImageBrightness( const QImage& image, int brightness );
QImage changeImageContrast( const QImage& image, int contrast );
QImage changeImageGamma( const QImage& image, int gamma );
QImage changeImageMax( const QImage& image, int min);
QImage changeImageMin( const QImage& image, int min);

#endif