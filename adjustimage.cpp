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

#include <QtCore>
#include <QImage>
#include "math.h"

template<class T> inline const T& kClamp( const T& x, const T& low, const T& high ) {
    if ( x < low )       return low;
    else if ( high < x ) return high;
    else                 return x;
}

inline int changeBrightness( int value, int brightness ) {
	return kClamp( value + brightness * 255 / 100, 0, 255 );
}

inline int changeContrast( int value, int contrast ) {
    return kClamp((( value - 127 ) * contrast / 100 ) + 127, 0, 255 );
}

inline int changeGamma( int value, int gamma ) {
    return kClamp( int( pow( value / 255.0, 100.0 / gamma ) * 255 ), 0, 255 );
}

int changeMax( int value, int max) {
	if(max == 0) {
		max = 1;
	}
	return kClamp( value  * 255 / max, 0, 255);
}

int changeMin( int value, int min) {
	if(min == 255){
		min = 254;
	}
	return kClamp( (value - min) / (1 - (min / 255)), 0, 255);
}

inline int changeUsingTable( int value, const int table[] ) {
    return table[ value ];
}

template< int operation( int, int ) >
static
QImage changeImage( const QImage& image, int value ) {
    QImage im = image;
    im.detach();
    if( im.numColors() == 0 ) /* truecolor */ {
        if( im.format() != QImage::Format_RGB32 ) /* just in case */
            im = im.convertToFormat( QImage::Format_RGB32 );
        int table[ 256 ];
        for(int i = 0;i < 256;++i)
            table[ i ] = operation( i, value );
        if(im.hasAlphaChannel()){
            for(int y = 0;y < im.height();++y){
                QRgb* line = reinterpret_cast< QRgb* >( im.scanLine( y ));
                for( int x = 0; x < im.width(); ++x ) {
                    line[ x ] = qRgba( changeUsingTable( qRed( line[ x ] ), table ),
                        changeUsingTable( qGreen( line[ x ] ), table ),
                        changeUsingTable( qBlue( line[ x ] ), table ),
                        changeUsingTable( qAlpha( line[ x ] ), table ));
				}
			}
		} else {
            for(int y = 0;y < im.height();++y){
                QRgb* line = reinterpret_cast< QRgb* >( im.scanLine( y ));
                for(int x = 0;x < im.width(); ++x ){
                    line[ x ] = qRgb( changeUsingTable( qRed( line[ x ] ), table ),
                        changeUsingTable( qGreen( line[ x ] ), table ),
                        changeUsingTable( qBlue( line[ x ] ), table ));
                }
			}
		}
    } else {
        QVector<QRgb> colors = im.colorTable();
        for(int i = 0;i < im.numColors();++i ){
            colors[ i ] = qRgb( operation( qRed( colors[ i ] ), value ),
                                operation( qGreen( colors[ i ] ), value ),
                                operation( qBlue( colors[ i ] ), value ));
		}
		im.setColorTable(colors);
	}
    return im;
}

QImage convertToPseudoColor(const QImage& image){
	QImage im = image;
    im.detach();
	if(image.numColors() == 0){
		qDebug() << "convertToPseudoColor() : Not yet implemented, no colortable.";
	} else {		
		QVector<QRgb> colors = im.colorTable();
		for(int i = 0;i < im.numColors();++i ){
			int value = (qRed(colors[i]) + qBlue(colors[i]) + qGreen(colors[i]) ) / 3;
			colors[i] = qRgb((sin(value * 2 * 3.141 / 255) + 1) * 255,
							 (cos(value * 2 * 3.141 / 255) + 1) * 255,
							 (-1 * sin(value * 2 * 3.141 / 255) + 1) * 255);
		}
		im.setColorTable(colors);
	}
	return im;
}

// brightness is multiplied by 100 in order to avoid floating point numbers
QImage changeImageBrightness( const QImage& image, int brightness ) {
    if( brightness == 0 ) // no change
        return image;
    return changeImage< changeBrightness >( image, brightness );
}


// contrast is multiplied by 100 in order to avoid floating point numbers
QImage changeImageContrast( const QImage& image, int contrast ) {
    if( contrast == 100 ) // no change
        return image;
    return changeImage< changeContrast >( image, contrast );
}

// gamma is multiplied by 100 in order to avoid floating point numbers
QImage changeImageGamma( const QImage& image, int gamma ) {
    if( gamma == 100 ) // no change
        return image;
    return changeImage< changeGamma >( image, gamma );
}

QImage changeImageMin( const QImage& image, int min) {
	return changeImage< changeMin >(image, min);
}

QImage changeImageMax( const QImage& image, int max) {
	return changeImage< changeMax >(image, max);
}

