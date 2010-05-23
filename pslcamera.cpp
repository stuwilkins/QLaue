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
#include "pslcamera.h"

PSLCameraThread::PSLCameraThread(QObject *parent)
: QThread(parent), quit(false) {
	hostName = "localhost";
	port = 50000;
}

PSLCameraThread::~PSLCameraThread() {
	mutex.lock();
	quit = true;
	cond.wakeOne();
	mutex.unlock();
	wait();
}

void PSLCameraThread::getImage(void) {
	qDebug() << "PSLCameraThread::getImage()";
	QMutexLocker locker(&mutex);
	if (!isRunning())
		start();
	else
		cond.wakeOne();
}

void PSLCameraThread::sendSnap(QString serverName, qint16 serverPort){
	const int Timeout = 100 * 1000;
	
	QTcpSocket socket;
	socket.connectToHost(serverName, serverPort);
	qDebug() << "PSLCameraThread::run() : connecting to " << serverName << ":" << serverPort;
	
	if (!socket.waitForConnected(Timeout)) {
		qDebug() << "PSLCameraThread::run() : Unable to connect";
		emit error(socket.error(), socket.errorString());
		return;
	}
	
	QTextStream in(&socket);
	
	in << "Snap" << endl;
	
	while (socket.bytesAvailable() < 4) {
		if (!socket.waitForReadyRead(Timeout)) {
			qDebug() << "PSLCameraThread::run() : Read Timeout";
			emit error(socket.error(), socket.errorString());
			return;
		}
	}
	
	QString reply;
	do {
		reply = in.readLine(4);
	} while (reply.isNull());
	
	if(reply != "True") {
		qDebug() << "PSLCameraThread::run() : Incorrect camera response " << reply;
		return;
	}
	
	socket.close();
}

void PSLCameraThread::getImage(){
	
}

void PSLCameraThread::run()
{
	mutex.lock();
	QString serverName = hostName;
	quint16 serverPort = port;
	mutex.unlock();
	
	// Send the "Snap" Command to the camera
	
	sendSnap(serverName, serverPort);
	
	QTcpSocket socket;
	const int Timeout = 60 * 1000;
	socket.connectToHost(serverName, serverPort);
	qDebug() << "PSLCameraThread::run() : connecting to " << serverName << ":" << serverPort;
	
	if (!socket.waitForConnected(Timeout)) {
		qDebug() << "PSLCameraThread::run() : Unable to connect";
		emit error(socket.error(), socket.errorString());
		return;
	}
	
	socket.write("GetImage\n");
	qDebug() << "PSLCameraThread::run() : Sent GetImage";
	
	while (socket.bytesAvailable() < 100) {
		if (!socket.waitForReadyRead(Timeout)) {
			qDebug() << "PSLCameraThread::run() : Read Timeout (GetImage)";
			emit error(socket.error(), socket.errorString());
			return;
		}
	}
	
	char textBuffer[256];
	qint64 ncr;
	ncr = socket.readLine(textBuffer, 20);
	qDebug() << ncr << textBuffer << endl;
	QString header(textBuffer);
	QStringList list = header.split(";");
	bool ok;
	int x = list[0].toInt(&ok);
	int y = list[1].toInt(&ok);
	int size = list[2].split("x")[0].toInt(&ok);
	qDebug() << "PSLCameraThread::run() : Image Size " << x << "x" << y << " (" << size << " bytes)";
	
	int dataStart = header.indexOf("x");
	if(dataStart == -1){
		qDebug() << "PSLCameraThread::run() : x not found in data header";
		return;
	}
	
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
				qDebug() << "PSLCameraThread::run() : Read Error " << socket.errorString();
				emit error(socket.error(), socket.errorString());
				return;
			}
		}
		ncr = socket.readLine(buf, 20);
		buf+= ncr;
		nr+= ncr;
	}
	
	QByteArray cimage(buffer, size + 4);
	QByteArray uncimage = qUncompress(cimage);
	if(uncimage.isEmpty()){
		qDebug() << "PSLCameraThread::run() : Error decompressing image";
	}
	
	QImage image(x, y, QImage::Format_RGB32);
	for(int j=0;j<y;j++){
		for(int i=0;i<x;i++){
			int k = uncimage[(2 * ((j * x) + i))];
			image.setPixel(i, j, qRgb(k,k,k));
		}
	}
	
	qDebug() << "PSLCameraThread::run() : Image Avaliable ( " << uncimage.size() << " bytes)";
	
	mutex.lock();		
	serverName = hostName;
	serverPort = port;
	receivedImage = image;
	emit newImage();
	mutex.unlock();
}