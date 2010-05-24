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
 
 $Revision: 24 $
 $Author: stuwilkins $
 $Date: 2010-05-20 08:22:17 -0400 (Thu, 20 May 2010) $
 $HeadURL: https://qlaue.svn.sourceforge.net/svnroot/qlaue/trunk/mainwindow.cpp $
 
 */

#include <QtNetwork>
#include <QTextStream>
#include <QColor>
#include <QPixmap>
#include "pslcamera.h"

PSLCameraImageDialog::PSLCameraImageDialog(QWidget* parent) :
QDialog(parent)
{
	ui.setupUi(this);
	setModal(true);
}

void PSLCameraImageDialog::setImage(QImage i){
	QPixmap pixmap = QPixmap::fromImage(i);
	QSize newsize = pixmap.size();
	newsize.scale(ui.image->size(), Qt::KeepAspectRatio);
	ui.image->setPixmap(pixmap.scaled(newsize));
}

PSLCameraDialog::PSLCameraDialog(QWidget* parent) :
QDialog(parent)
{
	ui.setupUi(this);
	setModal(true);
}

PSLCameraThread::PSLCameraThread(QObject *parent)
: QThread(parent), quit(false) {
	hostName = "localhost";
	port = 50000;
	timeout = 60 * 1000;
	integrationTime = 10;
	binning = 2;
}

PSLCameraThread::~PSLCameraThread() {
	wait();
}

void PSLCameraThread::setHost(QString host, qint16 portnum){
	mutex.lock();
	hostName = host;
	port = portnum;
	mutex.unlock();
}

void PSLCameraThread::setIntegrationTime(double time){
	mutex.lock();
	integrationTime = time;
	mutex.unlock();
}

void PSLCameraThread::setBinning(int bin){
	mutex.lock();
	binning = bin;
	mutex.unlock();
}

void PSLCameraThread::getImage(void) {
	qDebug() << "PSLCameraThread::getImage()";
	QMutexLocker locker(&mutex);
	if (!isRunning())
		start();
	else
		cond.wakeOne();
}

void PSLCameraThread::sendCommand(QString command){
	
	mutex.lock();
	QString serverName = hostName;
	quint16 serverPort = port;
	const int Timeout = timeout;
	mutex.unlock();
	
	QTcpSocket socket;
	socket.connectToHost(serverName, serverPort);
	qDebug() << "PSLCameraThread::sendCommand() : connecting to " << serverName << ":" << serverPort;
	
	if (!socket.waitForConnected(Timeout)) {
		qDebug() << "PSLCameraThread::sendCommand() : Unable to connect";
		emit error(socket.error(), socket.errorString());
		return;
	}
	
	QTextStream in(&socket);
	
	qDebug() << "PSLCameraThread::sendCommand() : Sending " << command ;  
	in << command << endl;
	
	while (socket.bytesAvailable() < 2) {
		if (!socket.waitForReadyRead(Timeout)) {
			qDebug() << "PSLCameraThread::sendCommand() : Read Timeout";
			emit error(socket.error(), socket.errorString());
			return;
		}
	}
	
	QString reply;
	do {
		reply = in.readLine(2);
	} while (reply.isNull());
	
	if(reply != "[]") {
		qDebug() << "PSLCameraThread::run() : Incorrect camera response " << reply;
		return;
	}
	
	socket.close();
}


void PSLCameraThread::sendSnap(void){
	
	mutex.lock();
	QString serverName = hostName;
	quint16 serverPort = port;
	int Timeout = timeout;
	mutex.unlock();
	
	emit statusChange(QString("Starting image acquisition ..."));
	
	QTcpSocket socket;
	socket.connectToHost(serverName, serverPort);
	qDebug() << "PSLCameraThread::sendSnap() : connecting to " << serverName << ":" << serverPort;
	
	if (!socket.waitForConnected(Timeout)) {
		qDebug() << "PSLCameraThread::sendSnap() : Unable to connect";
		emit error(socket.error(), socket.errorString());
		return;
	}
	
	QTextStream in(&socket);
	
	in << "Snap" << endl;
	QTime t;
	t.start();
	
	Timeout = 1000; // 100 msec
	while (socket.bytesAvailable() < 4) {
		if (!socket.waitForReadyRead(Timeout)) {
			if(socket.error() == QAbstractSocket::SocketTimeoutError){
				double pcomp = 100 * t.elapsed() / (integrationTime * 1000);
				emit statusChange(QString("Acquisition %1\% complete ...").arg(pcomp, 4, 'f', 1));
				if(pcomp > 120){
					qDebug() << "PSLCameraThread::sendSnap() : Server did not reply";
					break;
				}
			} else {
				qDebug() << "PSLCameraThread::sendSnap() : Error on reading reply";
				emit error(socket.error(), socket.errorString());
				return;
			}
		}
	}
	
	QString reply;
	do {
		reply = in.readLine(4);
	} while (reply.isNull());
	
	if(reply != "True") {
		qDebug() << "PSLCameraThread::sendSnap() : Incorrect camera response " << reply;
		return;
	}
	qDebug() << "PSLCameraThread::sendSnap() : Response recieved";
	
	socket.close();
}



bool PSLCameraThread::sendGetImage(void){
	
	mutex.lock();
	QString serverName = hostName;
	quint16 serverPort = port;
	const int Timeout = timeout;
	mutex.unlock();
	
	emit statusChange(QString("Retrieving image from server ..."));
	
	QTcpSocket socket;
	socket.connectToHost(serverName, serverPort);
	qDebug() << "PSLCameraThread::sendGetImage() : connecting to " << serverName << ":" << serverPort;
	
	if (!socket.waitForConnected(Timeout)) {
		qDebug() << "PSLCameraThread::sendGetImage() : Unable to connect";
		emit error(socket.error(), socket.errorString());
		return false;
	}
	
	socket.write("GetImage\n");
	qDebug() << "PSLCameraThread::sendGetImage() : Sent GetImage";
	
	while (socket.bytesAvailable() < 100) {
		if (!socket.waitForReadyRead(Timeout)) {
			qDebug() << "PSLCameraThread::sendGetImage() : Read Timeout (GetImage)";
			emit error(socket.error(), socket.errorString());
			return false;
		}
	}
	
	char textBuffer[256];
	qint64 ncr;
	ncr = socket.readLine(textBuffer, 30);
	
	qDebug() << "PSLCameraThread::sendGetImage() : " << ncr << textBuffer;
	
	QString header(textBuffer);
	QStringList list = header.split(";");
	bool ok;
	int x = list[0].toInt(&ok);
	int y = list[1].toInt(&ok);
	int size = list[2].split("x")[0].toInt(&ok);
	qDebug() << "PSLCameraThread::sendGetImage() : Image Size " << x << "x" << y << " (" << size << " bytes)";
	
	int dataStart = list[0].length() + list[1].length() + 2 + list[2].split("x")[0].length();

	char *buffer = new char[size + 4];
	int s = x * y * 2;
	
	buffer[0] = (char)((s >> 24) & 0xFF);
	buffer[1] = (char)((s >> 16) & 0xFF);
	buffer[2] = (char)((s >> 8) & 0xFF);
	buffer[3] = (char)((s >> 0) & 0xFF);
	
	char *buf = &buffer[4];
	
	for(int i=dataStart;i<ncr;i++){
		*buf = textBuffer[i];
		buf++;
	}
	
	int nr = ncr - dataStart;
	while(nr < size){
		while (socket.bytesAvailable() < 1) {
			if (!socket.waitForReadyRead(Timeout)) {
				qDebug() << nr << " " << socket.bytesAvailable();
				qDebug() << "PSLCameraThread::sendGetImage() : Read Error " << socket.errorString();
				emit error(socket.error(), socket.errorString());
				return false;
			}
		}
		ncr = socket.readLine(buf, 20);
		buf+= ncr;
		nr+= ncr;
	}
	
	QByteArray cimage(buffer, size + 4);
	
	QByteArray uncimage = qUncompress(cimage);
	if(uncimage.isEmpty()){
		qDebug() << "PSLCameraThread::sendGetImage() : Error decompressing image";
		return false;
	}
	
	QImage image(x, y, QImage::Format_RGB32);
	
	unsigned int min = 0xFFFF;
	unsigned int max = 0x0;
	for(int j=0;j<y;j++){
		for(int i=0;i<x;i++){
			unsigned int k = (unsigned char)uncimage[(2 * ((j * x) + i))];
			//qDebug() << "k = " << k;
			k = k + ((unsigned char)uncimage[(2 * ((j * x) + i)) + 1] << 8);
			if(k < min){
				min = k;
			}
			if(k > max){
				max = k;
			}
			image.setPixel(i, j, qRgb(k,k,k));
		}
	}
	
	
	
	for(int j=0;j<y;j++){
		for(int i=0;i<x;i++){
			unsigned int k = (unsigned char)uncimage[(2 * ((j * x) + i))];
			//qDebug() << "k = " << k;
			k = k + ((unsigned char)uncimage[(2 * ((j * x) + i)) + 1] << 8);
			k = (unsigned int)(0xFF * (k - min) / (max - min));
			image.setPixel(i, j, qRgb(k,k,k));
		}
	}
	
	qDebug() << "PSLCameraThread::sendGetImage() : Min = " << min << " Max = " << max;
	qDebug() << "PSLCameraThread::sendGetImage() : Image Avaliable ( " << uncimage.size() << " bytes)";
	
	// Now copy image 
	mutex.lock();		
	receivedImage = image;
	emit newImage();
	mutex.unlock();
	
	emit statusChange(QString("Done."));
	
	return true;
}

void PSLCameraThread::run()
{
	// Send the parameters for the camera
	sendCommand(QString("SetBinning;%1;%2").arg(binning).arg(binning));
	sendCommand(QString("SetExposure;%1").arg(integrationTime * 1000));
	
	// Send the "Snap" Command to the camera
	sendSnap();
	
	// Send the "GetImage" to the camera 
	sendGetImage();
	
}