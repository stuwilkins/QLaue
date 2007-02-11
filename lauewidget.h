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

#ifndef _LAUEFILM_H
#define _LAUEFILM_H

#include <QWidget>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QRubberBand>
#include "laue.h"
#include "crystal.h"

#ifdef CALC_IN_THREAD

class LaueThread : public QThread, public lauegram {
	Q_OBJECT
	Crystal* crystal;
	QMutex mutex;
	QWaitCondition condition;
	bool restart, abort;
public:
	LaueThread(QObject *parent = 0);
	~LaueThread();
	void startCalculation(Crystal *c);
protected:
	void run();
signals:
	void calculated(void);
	void partialCalc(void);
	void spotsToGo(int);
	void calcStarted(void);
};

class LaueIndexingThread : public QThread, public lauegram {
	Q_OBJECT
	Crystal newOrient;
	QMutex mutex;
	QWaitCondition condition;
	bool restart, abort;
	int numIndexSpots;
	double *indexSpotsX;
	double *indexSpotsY;
	int nOrientations;
	LaueOrientations *orientations;
	int niterations;
public:
	LaueIndexingThread(QObject *parent = 0);
	~LaueIndexingThread();
	void indexLaue(Crystal *crystal, int numIndexSpots, 
				   double *indexSpotsX, double *indexSpotsY);
	int triesToGo(void)						{ return niterations; }
	LaueOrientations getOrientations(int i) { return orientations[i]; }
	int getMaxIterations(void);
protected:
	void run();
signals:
	void newIndexingAvaliable(int nOrientations, LaueOrientations *orientations);
	void progress(int toGo);
	void calcStarted(void);
};
#else

class LaueThread : public QObject, public lauegram {
	Q_OBJECT
public:
	LaueThread(QObject *parent = 0);
	~LaueThread();
	void startCalculation(Crystal *c);
signals:
	void calculated(void);
	void partialCalc(void);
	void spotsToGo(int);
	void calcStarted(void);
};

#endif

class LaueFilm : 
public QWidget
{
	Q_OBJECT
private:	
	Crystal *crystal;
	LaueThread *laue;
	LaueIndexingThread *laueIndexing;
	
	LaueOrientations *orientations;
	int numOrientations;
	
	
	// Variables to control the user action
	
	int display;
	int userAction;
	int userActionStage;
	QPoint moveStart;
	
	// Variables to convert from world to pixels
	
	QPointF laueOrigin;
	QPointF imageOrigin;
	
	// Bitmap laue image variables
	
	bool imageInvert;
	double pixels_per_mm;
	
	// Laue parameters
	
	double origin_circle;
	int max_spot_size;
	
	int numOrientationSpots;
	int numUBOrientationSpots;
	double *orientationSpotsX;
	double *orientationSpotsY;
	double UBOrientationSpotsX[2];
	double UBOrientationSpotsY[2];
	
	QPointF rotateAboutAxisPoint;
	
	QString laueMessage;
	QRect laueRect;
	QRect messageRect;
	QRubberBand *rubberBand;
	QPoint rubberBandOrigin;
	QPoint importedImagePos;
	double importedImageDX;
	double importedImageDY;
	QPoint zoomImagePos;
	
	double hkl_labels[200][3];
	int num_hkl_labels;
	int mouseover_hkl_label;
	
	void paintLaue(QPainter *painter, QRect size);
	void paintImage(QPainter *painter, QRect size);
	void paintLaueAxes(QPainter *painter, QRect laue);
	void paintLaueBox(QPainter *painter, QRect laue);
	void paintMsgBox(QPainter *painter, QRect msg);
	void paintDummyFilm(QPainter *painter, QRect laue, QRect msg);
	void paintOrigin(QPainter *painter, QRect size);
	void paintIndexing(QPainter *painter, QRect size);
	void paintUBOrientationSpots(QPainter *painter, QRect size);
	void paintRotateAboutCross(QPainter *painter, QRect size);
	
	void addIndexLabel(double h, double k, double l);
	double pixelsToMm(int pixels);
	int mmToPixels(double mm);
	QPointF pixelsToWorld(QPoint pixel, QRect rect);
	QPoint  worldToPixels(QPointF world, QRect rect);
	
	void setBit(int *flags, int flag, int toset);
	bool getBit(int flags, int flag);
	
public:
	
	LaueFilm(QWidget *parent = 0, Crystal* crystal = 0);
	~LaueFilm();
	
	void setCrystal(Crystal* c)		{ crystal = c; }
	Crystal* getCrystal(void)		{ return crystal; }
	
	QImage *importedScaledImage;
	QImage *importedImage;
	
	LaueThread* getLaue(void)		{ return laue; }
	LaueIndexingThread* getIndexing(void)	{return laueIndexing; }
	LaueOrientations* getOrientations(void) {return orientations; }
	
	double originX(void)			{ return laueOrigin.x(); }
	double originY(void)			{ return laueOrigin.y(); }
	double scale(void)				{ return pixels_per_mm; }
	void setOriginX(double x)		{ laueOrigin.setX(x); }
	void setOriginY(double x)		{ laueOrigin.setY(x); }
	void setScale(double x)			{ pixels_per_mm = x; }
	
	enum {
		NoPrintReorientation = 0,
		PrintReorientation = 1
	};
	
	enum {
		UANone =					0x000,
		UASetOrigin =				0x001,
		UAMeasureScale =			0x002,
		UAMeasureZoom =				0x004,
		UASetXAxis =				0x008,
		UARotateX =					0x010,
		UARotateYZ =				0x020,
		UARotateAboutAxis =			0x040,
		UASelectOrientation =		0x080,
		UASelectUBOrientation =		0x100
	};
	
	enum {
		DisplayImage =				0x001,
		DisplayLaue =				0x002,
		DisplayIntensities =		0x004,
		DisplayOrigin =				0x008,
		DisplayLabels =				0x010,
		DisplayCharacteristic =		0x020,
		DisplayIntensitiesColor =	0x040,
		DisplayIndexing =			0x080,
		DisplayUBIndexing =			0x100,
		DisplayImageInverted =		0x200,
		DisplayRotateAboutCross =	0x400
	};
	
	enum {
		ImageNone =					0,
		ImageInvert =				0x001
	};
	
private slots:
	void displayAfterCalc(void);	
public slots:
	void print(QPrinter *printer, QPainter *painter, int mode);
	void setOrigin(void);
	void measureScale(void);
	void measureZoom(void);
	void rotateAboutAxis(void);
	void resetZoom(void);
	void setU(double h1, double k1, double l1,
			  double h2, double k2, double l2);
	void setU(Matrix U);
	void setFSDistance(double d);
	void setDisplayLaue(int yesno);
	bool getDisplayLaue(void);						
	void setDisplayIntensities(int yesno);
	bool getDisplayIntensities(void);				
	void setDisplayIntensitiesAsColor(bool yesno);	
	bool getDisplayIntensitiesAsColor(void);
	void setDisplayCharacteristicInt(int yesno);
	bool getDisplayCharacteristicInt(void);
	void setZoom(double zoom);
	void setSpotSize(int spotsize)					{ max_spot_size = spotsize; update(); }
	int  getSpotSize(void)							{ return max_spot_size; }
	void setInvertImage(bool yesno);
	void setDisplayImage(bool yesno);				
	void displayMessage(const QString message);
	void clearMessage(void);
	void setPixelsPerMMFromImage(void);
	void resetView(void);
	void setDetLevel(int);
	void setSatLevel(int);
	void resetLabels(void)							{ num_hkl_labels = 1; update(); }
	void showLabels(bool flag);
	void setAccuracy(int newval)					{ laue->setCalcPrecision(11 - newval); reCalc();}
	int  getAccuracy(void)							{ return (11 - laue->calcPrecision()); }
	void solveOrientation(void);
	void resolveOrientation(void);
	void clearOrientation(void);
	void setOrientation(void);
	void resetOrientation(void);
	void reCalc(void);
protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event); 
	void mouseReleaseEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
signals:
	void rotationsChanged(double xx, double yy, double zz);
	void recalculated(void);
	void gotFocus(void);
	void newIndexingAvaliable(int nOrientations, LaueOrientations *orientations);
};

#endif