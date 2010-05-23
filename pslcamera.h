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

#ifndef _PSLCAMERA_H
#define _PSLCAMERA_H

#include <QThread>
#include <QImage>
#include <QMutex>
#include <QWaitCondition>

class PSLCameraThread : public QThread
{
	Q_OBJECT
	
public:
	PSLCameraThread(QObject *parent = 0);
	~PSLCameraThread();
	void getImage(void);
	void run();
	QImage getNewImage() {return receivedImage;}
signals:
	void error(int socketError, const QString &message);
	void newImage(void);
private:
	void sendSnap(QString serverName, qint16 serverPort);
	QString hostName;
	quint16 port;
	QMutex mutex;
	QWaitCondition cond;
	QImage receivedImage;
	bool quit;
};

#endif