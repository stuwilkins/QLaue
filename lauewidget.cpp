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

#include <QtCore>
#include <QtGui>
#include <algorithm>

#include "matrix.h"
#include "version.h"
#include "laue.h"
#include "crystal.h"
#include "lauewidget.h"
#include "htmlgenerators.h"
#include "filmsizedialog.h"
#include "indexingdialog.h"
#include "setubmat.h"

#ifdef CALC_IN_THREAD

LaueIndexingThread::LaueIndexingThread(QObject *parent) :
QThread(parent), lauegram()
{
	restart = false;
	abort = false;
	
	// Now initialise the arrays for indexing spots
	
	orientations = new LaueOrientations[6500];
	indexSpotsX = new double[100];
	indexSpotsY = new double[100];
	
	nOrientations = 0;
	niterations = 0;
}

LaueIndexingThread::~LaueIndexingThread() {
	mutex.lock();
	abort = true;
	condition.wakeOne();
	mutex.unlock();
	
	wait();
	
	delete [] indexSpotsX;
	delete [] indexSpotsY;
	delete [] orientations;
}

int LaueIndexingThread::getMaxIterations(void) {

		int maxIterations = (getIndexingMaxH() * 2) + 1;
		maxIterations = maxIterations * ((getIndexingMaxH() * 2) + 1);
		maxIterations = maxIterations * ((getIndexingMaxH() * 2) + 1);
		maxIterations = maxIterations * maxIterations;
		return maxIterations;
}

void LaueIndexingThread::indexLaue(Crystal *crystal, 
								   int numIndexSpots, double *indexSpotsX, double *indexSpotsY){
								   
	QMutexLocker locker(&mutex);
	
	newOrient = *crystal;
	
	this->numIndexSpots = numIndexSpots;
	for(int i = 0 ; i < numIndexSpots ; i++){
		this->indexSpotsX[i] = indexSpotsX[i];
		this->indexSpotsY[i] = indexSpotsY[i];
	}
	
	this->nOrientations = nOrientations;
	this->orientations = orientations;
	
	if(!isRunning()) {
		start(LowPriority);
	} else {
		restart = true;
		condition.wakeOne();
	}
}

void LaueIndexingThread::run() {
	forever {
		
		nOrientations = 0;
		emit calcStarted();
		
		Matrix primary(getDistance(),-1.0 * indexSpotsX[0], indexSpotsY[0]);
		Matrix secondary(getDistance(), -1.0 * indexSpotsX[1] , indexSpotsY[1]);
		Matrix k_in(1,0,0);
		
		primary = k_in + (primary / primary.mag());
		secondary = k_in + (secondary / secondary.mag());
		
		double theta = cos(primary.dot(secondary) / (primary.mag() * secondary.mag()));
		
		newOrient.setFreeRotate(true);
		
		setCalcIntensities(false);
		
		// Calculate number of iterations;
		
		niterations = 0;
		
		for(int h1 = (-1 * getIndexingMaxH()) ; h1 <= getIndexingMaxH() ; h1++){
			for(int k1 = (-1 * getIndexingMaxK()) ; k1 <= getIndexingMaxK() ; k1++){
				for(int l1 = (-1 * getIndexingMaxL()) ; l1 <= getIndexingMaxL() ; l1++){
					for(int h2 = (-1 * getIndexingMaxH()) ; h2 <= getIndexingMaxH() ; h2++){
						for(int k2 = (-1 * getIndexingMaxK()) ; k2 <= getIndexingMaxK() ; k2++){
							for(int l2 = (-1 * getIndexingMaxL()) ; l2 <= getIndexingMaxL() ; l2++){
								emit progress(++niterations);
								if(isFundamental(h1,k1,l1)){
									if(isFundamental(h2,k2,l2)){
										// Check the angle between vectors
										
										if(restart)
											break;
										if(abort)
											return;
										
										Matrix vec1(h1,k1,l1);
										Matrix vec2(h2,k2,l2);
										
										vec1 = newOrient.getB() * vec1;
										vec1 = vec1 / vec1.mag();
										vec2 = newOrient.getB() * vec2;
										vec2 = vec2 / vec2.mag();
										
										double gamma = cos(vec1.dot(vec2));
										
										if(abs(gamma - theta) < getIndexingAngularTol()){
											// Ok so we have a match to primary and secondary
											// try all other reflections then
											//  cerr << "MATCH : (" << h1 << "," << k1 << "," << l1 << ") and (";
											//  cerr << h2 << "," << k2 << "," << l2 << ")" << endl;
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
												
												double diff = pow(pow(s.X() - indexSpotsX[1],2) + pow(s.Y() - indexSpotsY[1],2),0.5);
												
												if(diff < getIndexingSpotTol()){
													int spotcount;
													calc(&newOrient, &abort, &spotcount);
													
													int nFoundSpots = 2;
													double error = diff;
													
													for(int n = 2; n < numIndexSpots ; n++){
														// Check if there is a spot in this place
														int sn = findSpot(indexSpotsX[n], indexSpotsY[n], getIndexingSpotTol());
														if( sn != -1){
															nFoundSpots++;
															error = error + pow(pow(getSpot(sn)->X() - indexSpotsX[n],2) + pow(getSpot(sn)->Y() - indexSpotsY[n],2),0.5);
														}
													}
													
													// Now store the solution
													// cerr << "Orientations = " << (nOrientations) << endl;
													orientations[nOrientations].setU(newU);
													orientations[nOrientations].setNumMatches(nFoundSpots);							
													orientations[nOrientations].setPatternError(error / nFoundSpots);
													orientations[nOrientations].setP(h1,k1,l1);
													orientations[nOrientations].setS(h2,k2,l2);
													nOrientations++;
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
		
		sort(orientations, orientations + nOrientations);
		
		emit newIndexingAvaliable(nOrientations, orientations);
		
		mutex.lock();
		if(!restart)
			condition.wait(&mutex);
		restart = false;
		mutex.unlock();
	}
}

LaueThread::LaueThread(QObject *parent) :
QThread(parent), lauegram()
{
	restart = false;
	abort = false;
}

LaueThread::~LaueThread() {
	mutex.lock();
	abort = true;
	condition.wakeOne();
	mutex.unlock();
	
	wait();
}

void LaueThread::run(){
	
	forever {
	
		emit calcStarted();
		
		int maxH = (int)(2.0 * getMaxKVec() * crystal->getLatticeA() / calcPrecision());
		int maxK = (int)(2.0 * getMaxKVec() * crystal->getLatticeB() / calcPrecision());
		int maxL = (int)(2.0 * getMaxKVec() * crystal->getLatticeC() / calcPrecision());
		
		int spotn = 0;
		int spotstocalc = maxH * maxK * maxL * 8;
		int hklcount = 0;
		
		for(int h=0; h<=maxH; h++){
			for(int k=0; k<=maxK; k++){
				for(int l=0; l<=maxL; l++){
					
					int mask = ((h == 0) ? 1 : 0);
					mask = mask << ((k == 0) ? 1 : 0);
					mask = mask << ((l == 0) ? 1 : 0);
					
					for(int n=0;n<=7;n++){
						if( n & mask)
							continue;
						int th = h * ( (n & 1) ? -1 : 1);
						int tk = k * ( (n & 2) ? -1 : 1);
						int tl = l * ( (n & 4) ? -1 : 1);
						if(calcHKL(crystal, getSpot(spotn),th,tk,tl)){
							spotn++;
						}
					}
					
					if(abort){
						setNspots(spotn);
						emit partialCalc();
						return;
					}
					
					if(restart){
						setNspots(spotn);
						emit partialCalc();
						break;
					}
				}
			}
			if(calcPrecision() <= 2){
				int percentcomp = (int)(100.0 * (double)hklcount / (double)spotstocalc);
				hklcount+= 8 * maxL * maxK;
				emit spotsToGo(percentcomp);
			}
		}
		
		setNspots(spotn);
		emit calculated();
		mutex.lock();
		if (!restart)
			condition.wait(&mutex);
		restart = false;
		mutex.unlock();
	}
}

void LaueThread::startCalculation(Crystal *c){
	QMutexLocker locker(&mutex);
	crystal = c;
	if(!isRunning()) {
		start(NormalPriority);
	} else {
		restart = true;
		condition.wakeOne();
	}
}


#else

LaueThread::LaueThread(QObject *parent) :
QObject(parent), lauegram()
{
	return;
}

LaueThread::~LaueThread() {
	return;
}

void LaueThread::startCalculation(Crystal *c){
	calc(c);
	emit calculated();
}

#endif 


LaueFilm::LaueFilm(QWidget *parent, Crystal* c)
: QWidget(parent)
{	
	
	setMinimumSize(QSize(300,500));
	importedImage = new QImage;
	importedScaledImage = new QImage;
	importedImageDX = 0.0;
	importedImageDY = 0.0;
	
	// Initialize arrays
	
	orientationSpotsX = new double[100];
	orientationSpotsY = new double[100];
	
	// Setup laue class
	
	laue = new LaueThread;
	laueIndexing = new LaueIndexingThread;
	
	crystal = c;
	
	// Connect thread finished
	connect(laue, SIGNAL(calculated()), this, SLOT(displayAfterCalc()));
	connect(laue, SIGNAL(partialCalc()), this, SLOT(displayAfterCalc()));
	
	laue->setDebug(false);
	laue->setDistance(35.0);
	laue->setLambdaRange(0.3,3);
	
	reCalc();
	
	// Define the origin of the film
	
	laueOrigin = QPoint(0,0);
	origin_circle = 0.5;

	pixels_per_mm = 0.006; // This is approximately 172 micron pixel size for PSL (2 x 2 binning)
	rubberBand = 0;
	
	max_spot_size = 50;
	
	num_hkl_labels = 1;
	mouseover_hkl_label = 1;
	
	numOrientationSpots = 0;
	numUBOrientationSpots = 0;
	
	display = LaueFilm::DisplayLaue;
	display |= LaueFilm::DisplayIntensities;
	display |= LaueFilm::DisplayOrigin;
	display |= LaueFilm::DisplayLabels;
	display |= LaueFilm::DisplayCharacteristic;
	
	userAction = LaueFilm::UANone;
	
	laueMessage = QString("");
	
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
	
	// Stuff for possible matches
	
	orientations = new LaueOrientations[65000];
	numOrientations = 0;
	
	//grabKeyboard();
}

LaueFilm::~LaueFilm(){
	delete importedImage;
	delete importedScaledImage;
	delete [] orientations;
	delete [] orientationSpotsX;
	delete [] orientationSpotsY;
}

bool LaueFilm::getDisplayLaue(void){
	return getBit(display,LaueFilm::DisplayLaue);
}

void LaueFilm::setDisplayLaue(int yesno){
	setBit(&display, (int)LaueFilm::DisplayLaue, yesno);
	update();
}

bool LaueFilm::getDisplayIntensities(void){
	return getBit(display,LaueFilm::DisplayIntensities);
}

void LaueFilm::setDisplayIntensities(int yesno){
	setBit(&display, LaueFilm::DisplayIntensities, yesno);
	getLaue()->setCalcIntensities(yesno);
	reCalc();
	update();
}

void LaueFilm::setDisplayIntensitiesAsColor(bool yesno){ 
	setBit(&display, (int)LaueFilm::DisplayIntensitiesColor,yesno);
	update();
}

bool LaueFilm::getDisplayIntensitiesAsColor(void){ 
	return getBit(display,LaueFilm::DisplayIntensitiesColor);
}

void LaueFilm::setDisplayCharacteristicInt(int yesno){ 
	setBit(&display, (int)LaueFilm::DisplayCharacteristic,yesno); 
	update();
}

bool LaueFilm::getDisplayCharacteristicInt(void){
	return getBit(display, LaueFilm::DisplayCharacteristic);
}

void LaueFilm::setInvertImage(bool yesno){ 
	setBit(&display, (int)LaueFilm::DisplayImageInverted, yesno);
	update();
}

void LaueFilm::setDisplayImage(bool yesno){ 
	setBit(&display, LaueFilm::DisplayImage, yesno); 
	update();
}

void LaueFilm::showLabels(bool flag){ 
	setBit(&display, LaueFilm::DisplayLabels, flag);
	update();
}

void LaueFilm::paintEvent(QPaintEvent *) {
	
	QPainter painter(this);
	
	laueRect = QRect(10,50,width() - 20, height() - 60);
	messageRect = QRect(10,10,width() - 20, 30);
	
	paintDummyFilm(&painter, laueRect, messageRect);
	
	paintMsgBox(&painter, messageRect);
	paintLaueBox(&painter, laueRect);
	
	if(getBit(display,LaueFilm::DisplayImage))
		paintImage(&painter, laueRect);
	
	paintLaueAxes(&painter, laueRect);
	
	if(getBit(display,LaueFilm::DisplayLaue))
		paintLaue(&painter, laueRect);
	
	if(getBit(display, LaueFilm::DisplayOrigin))
		paintOrigin(&painter, laueRect);
		
	if(getBit(display, LaueFilm::DisplayIndexing))
		paintIndexing(&painter, laueRect);
	
	if(getBit(display,LaueFilm::DisplayUBIndexing))
		paintUBOrientationSpots(&painter, laueRect);
	
	if(getBit(display,DisplayRotateAboutCross))
		paintRotateAboutCross(&painter, laueRect);
}

void LaueFilm::print(QPrinter *printer, QPainter *painter, int mode){
	QRect laueR;
	QRectF text;
	QDateTime now = QDateTime::currentDateTime();
	
	bool isLandscape = printer->width() > printer->height();
	
	if(isLandscape){
		laueR = QRect(10,10,(printer->width() / 2) - 20, printer->height() - 20);
		text = QRectF((printer->width() / 2) + 10, 10, (printer->width() / 2) - 20, printer->height() - 20);
	} else {
		laueR = QRect(10,10,printer->width() - 20, (printer->height() / 2) - 20);
		text = QRectF(10,(printer->height() / 2) + 10, printer->width() - 20, (printer->height()/2) - 20);
	}
	
	paintLaueBox(painter, laueR);
	if(getBit(display,LaueFilm::DisplayImage))
		paintImage(painter, laueR);
	paintLaueAxes(painter, laueR);
	paintLaue(painter,laueR);
	paintOrigin(painter, laueR);

	// Draw Bounding Rectangle
	
	QRectF innertext = QRectF(0,0,
							  text.width() - (text.width() / 10),
							  text.height() - (text.width() / 10));
	
	painter->setPen(QColor(0,0,0));
	painter->setBrush(QColor(255,255,255));
	painter->drawRect(text);
	painter->resetMatrix();
	painter->translate(text.topLeft() + QPoint(text.width() / 20.0, text.width() / 20.0));
	
	QTextDocument textbox;
	textbox.documentLayout()->setPaintDevice(printer);
	textbox.setPageSize(innertext.size());
	QFont font = QApplication::font(); 
	font.setPointSize(8);
	textbox.setDefaultFont(font);
	
	QString html;
	html = "<html><body>";
	html += HtmlGenerators::appHeader();
	html += QString("<b>Printed on : %1</b><br>&nbsp;<br>")
		.arg(now.toString("dddd d MMMM yyyy hh:mm"));
 	html += HtmlGenerators::crystalToHtml(crystal, HtmlGenerators::CrystalBasic);
	html += HtmlGenerators::laueToHtml(laue);
	html += "<b>Crystal Orientation:</b><br>";
	html +=  HtmlGenerators::crystalOrientationToHtml(crystal);
	html += "&nbsp;<br>";
	if(mode == PrintReorientation){
		html += HtmlGenerators::crystalOrientationToHtml(&laue->reorientation);
	}
	html += "</body></html>";
	textbox.setHtml(html);
	
	QAbstractTextDocumentLayout::PaintContext ctx;
	ctx.clip = QRectF(0,0,innertext.width(), innertext.height());
	//painter->drawRect(ctx.clip);
	textbox.documentLayout()->draw(painter, ctx);
}

void LaueFilm::paintRotateAboutCross(QPainter *painter, QRect size){
	QPoint cross = worldToPixels(rotateAboutAxisPoint, size);
	int spot_size = (int)(max_spot_size * ((double)size.height() / 2000));
	
	painter->setBrush(Qt::NoBrush);
	painter->setPen(QColor("darkGreen"));
	
	QRect crossRect(0,0,spot_size,spot_size);
	QRect circleRect(0,0,spot_size / 2,spot_size / 2);

	crossRect.moveCenter(cross);
	circleRect.moveCenter(cross);
		
	if(size.contains(crossRect,true)) {
		painter->drawLine(crossRect.topLeft(), crossRect.bottomRight());
		painter->drawLine(crossRect.topRight(), crossRect.bottomLeft());
		painter->drawEllipse(circleRect);
	}
}

void LaueFilm::paintDummyFilm(QPainter *painter, QRect laue, QRect msg){
	painter->setPen(QColor(0,0,0));
	painter->setBrush(QColor(255,255,255));
	
	painter->drawRect(laue);
	painter->drawRect(msg);
}

void LaueFilm::paintMsgBox(QPainter *painter, QRect msg){
	
	painter->setRenderHint(QPainter::Antialiasing);
	QColor hourColor(255,0,0);
	
	painter->setPen(QColor(0,0,0));
	painter->setBrush(QColor(255,255,255));
	painter->drawRect(msg);
	
	// Draw Text for message area
	
	painter->drawText(msg, Qt::AlignCenter, laueMessage);
}

void LaueFilm::paintLaueBox(QPainter *painter, QRect laue){
	// Draw border polygon
	
	painter->setPen(QColor(0,0,0));
	painter->setBrush(QColor(255,255,255));
	painter->drawRect(laue);	
}

void LaueFilm::paintLaueAxes(QPainter *painter, QRect laue){
	
	painter->setRenderHint(QPainter::Antialiasing);
	QColor hourColor(255,0,0);
	
	// Draw Coordinate axes
	
	painter->setBrush(QColor(0,0,0));
	painter->setPen(QColor(0,0,0));
	
	int pad = laue.height() / 24;
	int length = laue.width() / 6;
	
	QPointF points[3] = {
        QPointF(0.0, 0.0),
        QPointF(pad / 4, pad / 4),
        QPointF(pad / -4, pad / 4)
    };
	
	painter->resetMatrix();
    painter->translate(pad,laue.bottom() - pad);
	
	painter->drawLine(0,0,0,-1 * length);
	painter->drawLine(0,0,length,0);
	painter->drawText(0,-1 * length,tr("Z"));
	painter->drawText(length, 0,tr("Y"));
	painter->translate(0,-1 * length);
	painter->drawPolygon(points, 3);
	
	painter->translate(length,length);
	painter->rotate(90);
	painter->drawPolygon(points, 3);
	
	painter->resetMatrix();
	painter->translate(laue.right() - pad,laue.bottom() - pad);
	
	painter->drawLine(0,0,0,-1 * pad / 2);
	
	double world_length = (double)length * 2 / (pixels_per_mm * laue.height()); 
	//painter->drawRect(-2 * length,0,0,-1 * pad);
	painter->drawLine(-2 * length,0,0,0);
	painter->drawLine(-2 * length,0,-2 * length,-1 * pad / 2);
	painter->drawText(-2 * length + 5,0,length*2 - 10,-1 * pad / 2,Qt::AlignCenter,
					  QString("%1 mm").arg(world_length,0, 'g', 3));

}

void LaueFilm::paintOrigin(QPainter *painter, QRect size){
	
	painter->setBrush(QColor(0,0,255));
	painter->resetMatrix();
		
	QPoint oxy = worldToPixels(QPointF(0,0),size);
	painter->setPen(QColor(0,0,255));
	painter->setBrush(Qt::NoBrush);
	painter->translate(oxy);
	painter->rotate(45);
	QRect centrect1 = QRect(0,0,size.height() * origin_circle / 10,size.height() * origin_circle / 10);
	QRect centrect2 = QRect(0,0,size.height() / 10,size.height() / 10);
	
	centrect1.moveCenter(QPoint(0,0));
	centrect2.moveCenter(QPoint(0,0));
	painter->drawLine(centrect2.topLeft(), centrect2.bottomRight());
	painter->drawLine(centrect2.topRight(), centrect2.bottomLeft());
	painter->drawEllipse(centrect1);
	
	painter->resetMatrix();
}

void LaueFilm::paintImage(QPainter *painter, QRect size)
{	
	painter->resetMatrix();
	
	// Get the image
	
	QImage image = *importedImage;
	
	painter->setClipRect(size,Qt::ReplaceClip);
	
	QImage newimage = importedImage->scaled(size.size(), Qt::KeepAspectRatio);
	*importedScaledImage = newimage;
	
	if(getBit(display, DisplayImageInverted))
		newimage.invertPixels();

	int xpad = (size.width() - newimage.width()) / 2;
	int ypad = (size.height() - newimage.height()) / 2;
	
	QRect paintsize;
	if(ypad == 0){
		paintsize = QRect(size.translated(xpad,0).topLeft(),
										size.translated(-1 * xpad, 0).bottomRight());
	} else {	
		paintsize = QRect(size.translated(0, ypad).topLeft(), 
										size.translated(0, -1 * ypad).bottomRight());
	}
	
	importedImagePos = paintsize.topLeft();
	painter->drawImage(importedImagePos, newimage);
	
	if(getBit(userAction, UASelectOrientation) || getBit(userAction, UASelectUBOrientation)){
		if(QRect(importedImagePos.x(), importedImagePos.y(), newimage.width(),newimage.height()).contains(zoomImagePos)){
			QRect cutSize = QRect(0,0,15,15);
			cutSize.moveCenter(zoomImagePos - importedImagePos);
			QImage zoomImage = newimage.copy(cutSize);
			
			// Draw the image
	
			painter->drawImage(size.topRight() - QPoint(110,-10), zoomImage.scaledToHeight(100,Qt::SmoothTransformation));
			
			// Now draw some crosshairs
			
			painter->setPen("Red");
			
			painter->drawLine(size.topRight() - QPoint(60,-20),
							  size.topRight() - QPoint(60,-100));
			painter->drawLine(size.topRight() - QPoint(100,-60),
							  size.topRight() - QPoint(20,-60));
		}
	}
	
	painter->setClipping(false);
	painter->setPen("Black");
	painter->setBrush(Qt::NoBrush);
	painter->drawRect(size);
}

void LaueFilm::paintIndexing(QPainter *painter, QRect size){
	// Draw orientation spots
	
	painter->setBrush(Qt::NoBrush);
	painter->setPen(QColor("darkMagenta"));
	
	for(int i=0 ; i < numOrientationSpots ; i++){
		int spot_size;
		spot_size = (int)(max_spot_size * ((double)size.height() / 2000));
		
		QRect spot(0,0,spot_size,spot_size);
		
		QPoint spotpos = worldToPixels(QPointF(orientationSpotsX[i], orientationSpotsY[i]), size); 
		
		spot.moveCenter(spotpos);
		
		if(size.contains(spot,true)){
			painter->drawEllipse(spot);
			painter->drawText(spot.bottomRight() + QPoint(0,spot.width() / 2) ,QString("%1").arg(i+1));
		}
	}
}

void LaueFilm::paintUBOrientationSpots(QPainter *painter, QRect size){
	// Draw orientation spots
	
	painter->setBrush(Qt::NoBrush);
	painter->setPen(QColor("darkMagenta"));
	
	for(int i=0 ; i < numUBOrientationSpots ; i++){
		int spot_size;
		spot_size = (int)(max_spot_size * ((double)size.height() / 2000));
		
		QRect spot(0,0,spot_size,spot_size);
		
		QPoint spotpos = worldToPixels(QPointF(UBOrientationSpotsX[i], UBOrientationSpotsY[i]), size); 
		
		spot.moveCenter(spotpos);
		
		if(size.contains(spot,true)){
			painter->drawEllipse(spot);
			if(i == 0)
				painter->drawText(spot.bottomRight() + QPoint(0,spot.width() / 2) ,QString("P"));
			else
				painter->drawText(spot.bottomRight() + QPoint(0,spot.width() / 2) ,QString("S"));
		}
	}
}

void LaueFilm::paintLaue(QPainter *painter, QRect size) {

	painter->setBrush(QColor(0,0,0));
	painter->setPen(QColor(0,0,0));
	painter->resetMatrix();
	QPoint cent = size.center();
	
	// Calculate the screensize
	
	if(getBit(display, LaueFilm::DisplayIntensities)){
		double screeny = size.height() / (2 * pixels_per_mm*size.height());
		double screenx = size.width() / (2 * pixels_per_mm*size.height());
		double screenymax = screeny;
		double screenymin = (-1.0 * screeny);
		double screenxmax = screenx;
		double screenxmin = (-1.0 * screenx);

		laue->calcInten(screenxmin, screenxmax, screenymin, screenymax);
	}
	
	QColor bremsColor(255,0,0,127);
	QColor charColor("darkBlue");
	
	// Set font for labels
	
	QFont font = QApplication::font(); 
	font.setPointSize(10);
	painter->setFont(font);
	int fontpad = painter->boundingRect(size,0,tr("(0 0 0)")).height() / 2;
	
	for(int i=0 ; i < laue->getNspots() ; i++){
		int spot_size;
		
		if(getBit(display, LaueFilm::DisplayIntensities)){
			if(!getBit(display, LaueFilm::DisplayIntensitiesColor)){
				spot_size = (int)(max_spot_size * ((double)size.height() / 4000) * sqrt(laue->getSpot(i)->detI()));
			} else {
				spot_size = (int)(max_spot_size * ((double)size.height() / 4000));
				int clevel = (int)((1 - laue->getSpot(i)->detI()) * 255);
				bremsColor = QColor(clevel,clevel,clevel);
			}
		} else {
			spot_size = (int)(max_spot_size * ((double)size.height() / 4000));
			bremsColor = QColor(255,0,0);
		}
		
		QRect spot(0,0,spot_size,spot_size);
		
		if(laue->getSpot(i)->characteristic() && getBit(display, LaueFilm::DisplayCharacteristic)){
			painter->setBrush(charColor);
			painter->setPen(charColor);
		} else {
			painter->setBrush(bremsColor);
			painter->setPen(bremsColor);
		}
		
		// draw the spots!
		QPoint spotpos = worldToPixels(QPointF(laue->getSpot(i)->X(), laue->getSpot(i)->Y()), size); 
		
		spot.moveCenter(spotpos - QPoint(2,0));
		
		if(size.contains(spot,true)){
			painter->setPen(Qt::NoPen);
			painter->drawEllipse(spot);
		
			painter->setBrush(bremsColor);
			painter->setPen(QColor("blue"));
			if(getBit(display, LaueFilm::DisplayLabels)){
				for(int j=mouseover_hkl_label;j<num_hkl_labels;j++){
					if((laue->getSpot(i)->H() == hkl_labels[j][0]) &&
					   (laue->getSpot(i)->K() == hkl_labels[j][1]) &&
					   (laue->getSpot(i)->L() == hkl_labels[j][2])){
						painter->drawText(spot.topRight() + QPoint(spot_size,fontpad), 
										  QString("(%1 %2 %3)")
										  .arg(laue->getSpot(i)->H())
										  .arg(laue->getSpot(i)->K())
										  .arg(laue->getSpot(i)->L()));
					}
				}
			}
		}
	}
}

void LaueFilm::setU(double h1, double k1, double l1,
					double h2, double k2, double l2){
	crystal->setU(h1,k1,l1,h2,k2,l2);
	reCalc();
	emit rotationsChanged(0,0,0);
}

void LaueFilm::setU(Matrix U){
	crystal->setU(U);
	reCalc();
	emit rotationsChanged(0,0,0);
}

void LaueFilm::setFSDistance(double d){
	laue->setDistance(d);
	reCalc();
}

void LaueFilm::setZoom(double zoom){
	update();
}

void LaueFilm::mousePressEvent(QMouseEvent *event){

	if(getBit(userAction,LaueFilm::UASelectOrientation)) {
		
		if(laueRect.contains(event->pos())){
			QPointF spot = pixelsToWorld(event->pos(), laueRect);
			orientationSpotsX[numOrientationSpots] = (double)spot.x();
			orientationSpotsY[numOrientationSpots] = (double)spot.y();
			numOrientationSpots++;
			update();
		}
		
		event->accept();
		return;
	}
	
	if(getBit(userAction, LaueFilm::UASelectUBOrientation)) {
		if(laueRect.contains(event->pos())){
			QPointF spot = pixelsToWorld(event->pos(), laueRect);
			UBOrientationSpotsX[numUBOrientationSpots] = (double)spot.x();
			UBOrientationSpotsY[numUBOrientationSpots] = (double)spot.y();
			numUBOrientationSpots++;
			update();
		}
		
		if(numUBOrientationSpots == 2){
			userAction = UANone;
			displayMessage(QString(""));
			update();
			resetOrientation();
		}
		
		event->accept();
		return;
	}

	if(getBit(userAction,LaueFilm::UAMeasureZoom) && (userActionStage == 1)){
		setMouseTracking(true);
		rubberBandOrigin = event->pos(); // origin is a QPoint
        if(rubberBand)
			delete rubberBand;
		rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        rubberBand->setGeometry(QRect(rubberBandOrigin, QSize()));
        rubberBand->show();
		userActionStage = 2;
		return;
	}
	
	if(getBit(userAction,LaueFilm::UASetOrigin)){
		// We were asked to define the origin 
		laueOrigin = (event->pos() - laueRect.center());
		laueOrigin /= laueRect.height();
		userAction = LaueFilm::UANone;
		setCursor(Qt::ArrowCursor);
		setMouseTracking(true);
		setBit(&display, LaueFilm::DisplayLaue, true);
		setBit(&display, LaueFilm::DisplayOrigin, true);
		clearMessage();
		update();
		return;
	}
	
	if(getBit(userAction,LaueFilm::UAMeasureScale)){
		setMouseTracking(true);
		rubberBandOrigin = event->pos(); // origin is a QPoint
		if(rubberBand)
			delete rubberBand;	
		rubberBand = new QRubberBand(QRubberBand::Line, this);
		rubberBand->setGeometry(QRect(rubberBandOrigin, QSize()));
		rubberBand->show();
		userActionStage = 2;
		return;
	}
	
	if(getBit(userAction, UARotateAboutAxis)){
		if(userActionStage == 1){
			userActionStage = 2;
			displayMessage(QString("Use mouse to rotate about axis"));
			rotateAboutAxisPoint = pixelsToWorld(event->pos(), laueRect);
			setBit(&display, DisplayRotateAboutCross, true);
			setCursor(Qt::ArrowCursor);
			update();
		} else if(userActionStage == 2){
			userActionStage = 3;
			moveStart = event->pos();
		}
		event->accept();
		return;
	}
	
	if(QApplication::keyboardModifiers()  == Qt::ShiftModifier){
		QPointF spot = pixelsToWorld(event->pos(), laueRect);
		int snum = laue->findSpot(spot.x(),spot.y(),1);

		if(snum != -1){
			addIndexLabel(laue->getSpot(snum)->H(), laue->getSpot(snum)->K(), laue->getSpot(snum)->L());
			update();
		}
		event->accept();
		return;
	}
	
	moveStart = event->pos();
	
	if(event->button() == Qt::RightButton){
		setBit(&userAction, UARotateX, true);
	} else if(event->button() == Qt::LeftButton){
		setBit(&userAction, UARotateYZ, true);
	} else {
		userAction = UANone;
	}
		
	//qDebug("LaueFilm::mousePressEvent() : userAction = %d",userAction);
	event->accept();
	return;
}

void LaueFilm::mouseReleaseEvent(QMouseEvent *event){

	if(getBit(userAction, UARotateX)) {
		userAction = UANone;
		event->accept();
		return;
	}
	
	if(getBit(userAction, UARotateYZ)) {
		userAction = UANone;
		event->accept();
		return;
	} 

	if(getBit(userAction, UARotateAboutAxis) && (userActionStage == 3)){
		userAction = UANone;
		setBit(&display, DisplayRotateAboutCross, false);
		update();
		event->accept();
		return;
	}

	if(getBit(userAction,LaueFilm::UAMeasureScale)){
		userAction = LaueFilm::UANone;
		
		rubberBand->hide();
		// Last set of coordinates
		
		QPoint dxy = event->pos() - rubberBandOrigin;
		
		FilmSizeDialog dialog;
		if(dialog.exec() == QDialog::Accepted){
			double reallength = dialog.getFilmWidth();
			double pxlength = dxy.x();
			pixels_per_mm = (pxlength / reallength) / (laueRect.height());
		}
		
		setCursor(Qt::ArrowCursor);
		setMouseTracking(true);
		setBit(&display, LaueFilm::DisplayLaue, true);
		setBit(&display, LaueFilm::DisplayOrigin, true);
		update();
		event->accept();
		return;
	}
}

void LaueFilm::setPixelsPerMMFromImage(void){
	pixels_per_mm = importedImage->dotsPerMeterX() / 1000;
}

void LaueFilm::mouseMoveEvent(QMouseEvent *event){
	
	//qDebug("LaueFilm::mouseMoveEvent() : userAction = %d",userAction);
	
	if(	getBit(userAction,LaueFilm::UAMeasureZoom) || 
		getBit(userAction,LaueFilm::UAMeasureScale)){
		
		rubberBand->setGeometry(QRect(rubberBandOrigin, event->pos()).normalized());
		event->accept();
		return;
	}
	
	if(getBit(userAction,LaueFilm::UASetOrigin)) {
		if(laueRect.contains(event->pos())){
			laueOrigin = (event->pos() - laueRect.center());
			laueOrigin /= laueRect.height();
			update();
			event->accept();
			return;
		}
	}
	
	if(getBit(userAction, UARotateYZ)){
		
		QPointF moveYZ = pixelsToWorld(event->pos(), laueRect) - pixelsToWorld(moveStart, laueRect);
		
		double angley = atan(moveYZ.y() / laue->getDistance()) / 2;
		double anglex = atan(moveYZ.x() / laue->getDistance()) / 2;
		
		emit rotationsChanged(0, angley, anglex);
		
		moveStart = event->pos();
		
	} else if(getBit(userAction, UARotateX)){
		
		QPointF stopX = pixelsToWorld(event->pos(), laueRect);
		QPointF startX = pixelsToWorld(moveStart, laueRect);
		
		double dot = (startX.x() * stopX.x()) + (startX.y() * stopX.y());
		double mag = sqrt(pow((double)startX.x(),2) + pow((double)startX.y(),2)) 
				   * sqrt(pow((double)stopX.x(),2) + pow((double)stopX.y(),2));
		
		int cross = (startX.x() * stopX.y()) - (startX.y() * stopX.x());
		double angle = acos(dot / mag);
		
		if(cross > 0)
			angle = -1.0 * angle;

		emit rotationsChanged(angle, 0.0, 0.0);
		moveStart = event->pos();
		
	} else if (getBit(userAction, UARotateAboutAxis)) {
		if(userActionStage == 3){
			QPointF stopX = pixelsToWorld(event->pos(), laueRect) - rotateAboutAxisPoint;
			QPointF startX = pixelsToWorld(moveStart, laueRect) - rotateAboutAxisPoint;
			
			double dot = (startX.x() * stopX.x()) + (startX.y() * stopX.y());
			double mag = sqrt(pow((double)startX.x(),2) + pow((double)startX.y(),2)) 
				   * sqrt(pow((double)stopX.x(),2) + pow((double)stopX.y(),2));
		
			int cross = (startX.x() * stopX.y()) - (startX.y() * stopX.x());
			double angle = acos(dot / mag);
		
			if(cross > 0)
				angle = -1.0 * angle;
			
			Matrix pri = laue->filmposToVector(rotateAboutAxisPoint.x(),rotateAboutAxisPoint.y());
			crystal->rotateAboutBy(pri, angle);
			moveStart = event->pos();
			emit rotationsChanged(0.0 ,0.0 , 0.0);
			update();
		}
		event->accept();
		return;
	} else if(getBit(userAction, UASelectOrientation) || getBit(userAction, UASelectUBOrientation)) {
		setCursor(Qt::CrossCursor);
		// If we are displaying an image then cut out 50 pixels square
		// and send this pixmap as a signal.
		zoomImagePos = event->pos();	
		repaint();
	} else {
		// Is the mouse inside the film ?
		
		if(laueRect.contains(event->pos())){
		
			QPointF spot = pixelsToWorld(event->pos(), laueRect);
			
			int snum = laue->findSpot(spot.x(),spot.y(),1);
		
			if(snum == -1){
				QString msg = QString("X = %1 mm  Y = %2 mm")
								.arg(spot.x(),4,'f',2)
								.arg(spot.y(),4,'f',2);
				displayMessage(msg);
				setCursor(Qt::CrossCursor);
				mouseover_hkl_label = 1;
				update();
			} else {
				// Spot found
				QString msg = QString("Spot Index (%1, %2, %3)    Multiplicity %4    Intensity %5")
								.arg(laue->getSpot(snum)->H())
								.arg(laue->getSpot(snum)->K())
								.arg(laue->getSpot(snum)->L())
								.arg(laue->getSpot(snum)->getMaxdo())
								.arg(laue->getSpot(snum)->detI(), 3, 'f', 2);
				displayMessage(msg);
				setCursor(Qt::CrossCursor);
				mouseover_hkl_label = 0; // Add this spot now to the hkl_labels
				hkl_labels[0][0] = laue->getSpot(snum)->H();
				hkl_labels[0][1] = laue->getSpot(snum)->K();
				hkl_labels[0][2] = laue->getSpot(snum)->L();
				
				//cerr << getCrystal()->getU() * laue->filmposToVector(spot.x(), spot.y());
				update();
			}
		} else {
			setCursor(Qt::ArrowCursor);
		}
	}
}

void LaueFilm::keyPressEvent(QKeyEvent *event){
	if(event->key() == Qt::Key_Escape){
		
		if(getBit(userAction, UASelectOrientation)){
			userAction = UANone;
			displayMessage(QString("Calculating .."));
			update();
			
			resolveOrientation();
			
			event->accept();
			displayMessage(QString(""));
			return;
		} else {
			userAction = LaueFilm::UANone;
			displayMessage(QString(""));
			event->accept();
			update();
			return;
		}
	}
	
	if(getBit(userAction,LaueFilm::UASetOrigin)){
		if(event->key() == Qt::Key_Up){
			if(origin_circle <= 1) {
				origin_circle += 0.04;
			}
		}
		if(event->key() == Qt::Key_Down){
			if(origin_circle >= 0) {
				origin_circle -= 0.04;
			}
		}
		update();
		event->accept();
		return;
	}
	
	bool flag = true;
	double step = 0.025;
	
	if(QApplication::keyboardModifiers()  == Qt::ShiftModifier){
		step = step * 10;
	}
	
	double xrot = 0;
	double yrot = 0;
	double zrot = 0;

	switch(event->key()){
		case Qt::Key_Up:
			yrot+=step;
			break;
		case Qt::Key_Down:
			yrot-=step;
			break;
		case Qt::Key_Left:
			zrot-=step;
			break;
		case Qt::Key_Right:
			zrot+=step;
			break;
		case Qt::Key_Z:
			xrot+=(2 * step);
			break;
		case Qt::Key_X:
			xrot-=(2 * step);
			break;
		default:
			//qDebug("LaueFilm::keyPressEvent() : Unknown Key %d",event->key());
			flag = false;
			break;
	}
	
	if(flag){
		emit rotationsChanged(xrot, yrot, zrot);
		event->accept();
	} else {
		event->ignore();
	}
}

QPoint LaueFilm::worldToPixels(QPointF world, QRect rect){
	int x = (int)(world.x()*pixels_per_mm*rect.height());
	int y = (int)(world.y()*pixels_per_mm*rect.height());
				
	QPoint pixel = QPoint((int)((x + rect.center().x() + (int)(laueOrigin.x() * rect.height()))),
						  (int)((y * -1) + rect.center().y() + (int)(laueOrigin.y() * rect.height())));
	return pixel;
}

QPointF LaueFilm::pixelsToWorld(QPoint pixel, QRect rect){
	int xcent = (int)((laueOrigin.x()) * rect.height()) + rect.center().x();
	int ycent = (int)((laueOrigin.y()) * rect.height()) + rect.center().y();
	
	QPointF world((double)(pixel.x() - xcent) / (pixels_per_mm * rect.height()),
				  (double)(pixel.y() - ycent) / (-1 * pixels_per_mm * rect.height()));
	return world;
}

void LaueFilm::rotateAboutAxis(void) {
	// This only works in free rotate 
	// so check, and if not warn with a message box.
	
	if(!crystal->getFreeRotate()) {
		QMessageBox::information(this, tr(APP_NAME),
			tr("You cannot rotate about an axis when set to goniometer rotations. Please select \"free rotate\" and try again."));
		return;
	}

	setBit(&userAction, UARotateAboutAxis, true);
	userActionStage = 1;
	displayMessage(QString("Click on point to rotate about."));
	setCursor(Qt::CrossCursor);
}

void LaueFilm::setOrigin(void) {
	userAction = LaueFilm::UASetOrigin;
	setBit(&display, LaueFilm::DisplayLaue, false);
	setBit(&display, LaueFilm::DisplayOrigin, true);
	setCursor(Qt::BlankCursor);
	setMouseTracking(true);
	displayMessage(QString("Click in window to define origin."));
}

void LaueFilm::measureScale(void) {
	userAction = LaueFilm::UAMeasureScale;
	setCursor(Qt::CrossCursor);
	setMouseTracking(false);
	displayMessage(QString("Click and drag to measure."));
	setBit(&display, LaueFilm::DisplayLaue, false);
	setBit(&display, LaueFilm::DisplayOrigin, false);
	update();
}

void LaueFilm::setImageScale(void) {
	bool ok;
	
	double pixelSize = 1000 * importedScaledImage->width();
	pixelSize = pixelSize / (importedImage->width() * pixels_per_mm * laueRect.height());
	
	
	pixelSize = QInputDialog::getDouble(this, tr("QLaue"),
											tr("Pixel size (microns)"), pixelSize, 0, 20000, 2, &ok);
	if(ok){
		// Origional Image length in mm
		pixels_per_mm = importedImage->width() *  (pixelSize / 1000.0);
		qDebug() << QString("LaueFilm::setImageScale() : Image width = %1 mm").arg(pixels_per_mm);
		// Actual width of scaled image
		pixels_per_mm = importedScaledImage->width() / pixels_per_mm;
		pixels_per_mm = pixels_per_mm / laueRect.height();
		qDebug() << QString("LaueFilm::setImageScale() : pixels_per_mm = %1").arg(pixels_per_mm);
	}	
}

void LaueFilm::measureZoom(void) {
	userAction = LaueFilm::UAMeasureZoom;
	setCursor(Qt::CrossCursor);
	displayMessage(QString("Click and drag to zoom."));
	setMouseTracking(false);
	update();
}

void LaueFilm::resetZoom(void) {
	/*
	*importedScaledImage = *importedImage;
	imageox = 0; imageoy = 0;
	image_zoom = 1;
	update();
	*/
}

void LaueFilm::displayMessage(const QString message){
	laueMessage = message;
	update();
}

void LaueFilm::clearMessage(void){
	laueMessage = QString("");
	update();
}

void LaueFilm::resetView(void){
	laueOrigin = QPointF(0,0);
	pixels_per_mm = 0.006;
	update();
}

void LaueFilm::setDetLevel(int dl){
	laue->setDetLevel((double)dl / 100.0);
	update();
}

void LaueFilm::setSatLevel(int ds){
	laue->setDetSatLevel((double)ds / 100.0);
	update();
}

void LaueFilm::addIndexLabel(double h, double k, double l){
	for(int i=1;i<num_hkl_labels;i++){
		if((h == hkl_labels[i][0]) && (k == hkl_labels[i][1]) && (l == hkl_labels[i][2])){
			return;
		}
	}
	hkl_labels[num_hkl_labels][0] = h;
	hkl_labels[num_hkl_labels][1] = k;
	hkl_labels[num_hkl_labels][2] = l;
	num_hkl_labels++;
}

void LaueFilm::reCalc(void){
	laue->startCalculation(crystal);
}

void LaueFilm::displayAfterCalc(void){
	update();
	emit recalculated();
}

void LaueFilm::resolveOrientation(void){
	// First check to se if we have two or more
	// spots and if not we display a message box
	
	if(numOrientationSpots >= 2){

		IndexingDialog dialog(this);
		dialog.setIndexingParams(laueIndexing);
		if(dialog.exec() == QDialog::Accepted){
		
			dialog.getIndexingParams(laueIndexing);
			
			laueIndexing->setDistance(laue->getDistance());
			laueIndexing->setLambdaRange(laue->getLambdaRangeMax(), laue->getLambdaRangeMin());
			laueIndexing->setCalcPrecision(laue->calcPrecision());
			
			// Show progress
			
			int maxIterations = laueIndexing->getMaxIterations();
			QProgressDialog progress("Indexing Laue pattern. Please wait...", 
									 "Abort", 0, maxIterations, this);
			progress.setWindowModality(Qt::WindowModal);
			progress.setAutoReset(true);
			progress.setValue(0);
			
			laueIndexing->indexLaue(crystal, numOrientationSpots, orientationSpotsX, orientationSpotsY);
			
			int tries = 0;
			while((tries < maxIterations) && !progress.wasCanceled()) {
				qApp->processEvents();
				tries = laueIndexing->triesToGo();
				progress.setValue(tries);
			} 
			
			setBit(&display, LaueFilm::DisplayIndexing, true);
			
		} else {
			
			setBit(&display, LaueFilm::DisplayIndexing, false);
		
		}	
	
	} else {
		QMessageBox::warning(this, tr(APP_NAME), 
					tr("You have to select at least two reflections to index the Lauegram."), QMessageBox::Ok);
		numOrientationSpots = 0;
		setBit(&display, LaueFilm::DisplayIndexing, false);
	}
	
	setBit(&display, LaueFilm::DisplayLaue, true);
	update();
	return;
}

void LaueFilm::solveOrientation(void){
	
	setBit(&userAction, UASelectOrientation, true);
	numOrientationSpots = 0;
	setBit(&display, LaueFilm::DisplayLaue, false);
	setBit(&display, LaueFilm::DisplayIndexing, true);

	setCursor(Qt::BlankCursor);
	setMouseTracking(true);
	displayMessage(QString("Select Laue spots for indexing (Esc. to stop)."));
}

void LaueFilm::clearOrientation(void) {
	numOrientationSpots = 0;
	setBit(&display, LaueFilm::DisplayIndexing, false);
}

void LaueFilm::setOrientation(void) {
	setBit(&userAction, UASelectUBOrientation,true);
	numUBOrientationSpots = 0;
	setBit(&display, LaueFilm::DisplayIndexing, false);
	setBit(&display, LaueFilm::DisplayUBIndexing, true);
	
	setCursor(Qt::BlankCursor);
	setMouseTracking(true);
	displayMessage(QString("Select Two Laue spots for Indexing."));
}

void LaueFilm::resetOrientation(void) {
	UMatrixDialog dialog;
	if(dialog.exec() == QDialog::Accepted){
		Matrix pri = laue->filmposToVector(UBOrientationSpotsX[0],UBOrientationSpotsY[0]);
		Matrix sec = laue->filmposToVector(UBOrientationSpotsX[1],UBOrientationSpotsY[1]);
		
		getCrystal()->setU(pri,sec,dialog.getPrimary(), dialog.getSecondary());
		reCalc();
	} else {
		setBit(&display, DisplayUBIndexing, false);
	}
	
	setBit(&display, LaueFilm::DisplayLaue, true);
	displayMessage(QString(""));
}

void LaueFilm::setBit(int *flags, int flag, int toset){
	if(toset)
		*flags |= flag;
	else
		*flags &= ~flag;
}

bool LaueFilm::getBit(int flags, int flag){
	if((flags & flag) == flag)
		return true;
	else
		return false;
}
