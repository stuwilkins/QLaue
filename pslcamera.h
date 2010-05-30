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
#include "ui_pslcamera.h"
#include "ui_pslcamera2.h"
#include "ui_pslcamera3.h"

class PSLCameraWidget : public QWidget
{
	Q_OBJECT
	
public:
	PSLCameraWidget(QWidget *parent = 0);
private:
	Ui::PSLCameraWidget ui;
};


class PSLCameraDialog : public QDialog {
	Q_OBJECT
private:
	Ui::PSLCameraDialog ui;
public:
	PSLCameraDialog(QWidget* parent = 0);
	
	QString getHostname(void)				{ return ui.hostname->text(); }
	quint16 getPort(void)					{ return (quint16)ui.portSpinBox->value(); }
	double getIntegrationTime(void)			{ return ui.integrationTime->text().toDouble(); }
	int getBinning(void)					{ return (int)ui.binningSpinBox->value(); }
	
	void setHostname(QString hostname)		{ ui.hostname->setText(hostname); }
	void setPort(quint16 port)				{ ui.portSpinBox->setValue((int)port); }
	void setIntegrationTime(double time)	{ ui.integrationTime->setText(QString("%1").arg(time)); }
	void setBinning(int bin)				{ ui.binningSpinBox->setValue(bin); }
};

class PSLCameraImageDialog : public QDialog {
	Q_OBJECT
private:
	Ui::PSLCameraImageDialog ui;
public:
	PSLCameraImageDialog(QWidget* parent = 0);
	void setImage(QImage i);
};

class PSLCameraThread : public QThread
{
	Q_OBJECT
	
public:
	PSLCameraThread(QObject *parent = 0);
	~PSLCameraThread();
	void getImage(void);
	void run();
	QImage getNewImage() {return receivedImage;}
	
	void setHost(QString host, qint16 portnum);
	void setIntegrationTime(double time);
	void setBinning(int binning);
	
	QString getHostname(void)				{ return hostName; }
	quint16 getPort(void)					{ return port; }
	double getIntegrationTime(void)			{ return integrationTime; }
	int getBinning(void)					{ return binning; }
	
signals:
	void error(int socketError, const QString &message);
	void newImage(void);
	void statusChange(QString message);
	
private:
	void sendSnap(void);
	bool sendGetImage(void);
	void sendCommand(QString command);
	
	QString hostName;
	quint16 port;
	int timeout;
	double integrationTime;
	int binning;
	
	QMutex mutex;
	QWaitCondition cond;
	QImage receivedImage;
	bool quit;
};

#endif