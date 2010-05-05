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
#include <QtNetwork>
#include "httpcheck.h"
#include "version.h"
#include "svn.h"

#ifdef Q_WS_WIN
#include <windows.h>
#endif
#ifdef Q_WS_MAC
#include <Carbon/Carbon.h>
#endif

HttpCheck::HttpCheck(QWidget *parent) : QDialog(parent){
	ui.setupUi(this);
	ui.imageLabel->setPixmap(QPixmap(":crystal.png"));
	
	version = 0;
	revision = 0;
	
	http = new QHttp(this);	
	connect(http, SIGNAL(requestFinished(int, bool)),
			this, SLOT(httpRequestFinished(int, bool)));
	
#ifdef Q_WS_MAC
	
#endif
}

void HttpCheck::checkVersion(void){
	QUrl url(APP_VERSION_URL);
	//http->setProxy("proxy1.esrf.fr",3128);
	http->setHost(url.host(), url.port() != -1 ? url.port() : 80);
	httpGetId = http->get(url.path(), 0);
}

void HttpCheck::httpRequestFinished(int requestId, bool error){
	
	if (requestId != httpGetId)
		return;

	if(error){
		qDebug("HttpCheck::httpRequestFinished() : Error %s", qPrintable(http->errorString()));
		return;
	}
	
#ifdef APP_CURRENT_VERSION	
	version = APP_CURRENT_VERSION;
#endif
#ifdef SVN_REVISION
	revision = QString(SVN_REVISION).section(':',1,2).toInt();
#endif

	QList<QByteArray> tokens;
	tokens = http->readAll().split('\n');
	
	for(int t=0;t< tokens.size();t++){
		QList<QByteArray> elements;
		elements = tokens.at(t).split(',');
		if(elements.at(0) == "VERSION"){
			version = elements.at(1).toDouble();
		}
		
		if(elements.at(0) == "REVISION"){
			revision = elements.at(1).toInt();
		}
	}
	
	qDebug("HttpCheck::httpRequestFinished() : version = %lf revision = %d", version, revision);

#ifdef APP_CURRENT_VERSION
	if(version > APP_CURRENT_VERSION)
		showDialog();
#endif	
	
#ifdef SVN_REVISION
	if(revision > QString(SVN_REVISION).section(':',1,2).toInt())
		showDialog();
#endif
}

void HttpCheck::showDialog(void){

	QString message;
#ifdef SVN_REVISION	
	message = QString("You are currently running version %1.%2 of %3.\n\n")
		.arg(APP_CURRENT_VERSION).arg(QString(SVN_REVISION).section(':',1,2).toInt())
		.arg(APP_NAME);
#else
	message = QString("You are currently running version %1 of %2.\n\n")
		.arg(APP_CURRENT_VERSION)
		.arg(APP_NAME);
#endif
	message += QString("Version %1.%2 is now avaliable for download. Would you like to download it now?")
		.arg(version).arg(revision);
	
	ui.textLabel->setText(message);
	if(this->exec() != QDialog::Accepted)
		return;
	
	QString url = QString(APP_URL_TO_LOAD);
	
#ifdef Q_WS_WIN
    // Running in an MS Windows environment
    QT_WA( {
      ShellExecute(winId(), 0, (TCHAR*)url.utf16(), 0, 0, SW_SHOWNORMAL );
    } , {
      ShellExecuteA( winId(), 0, url.toLocal8Bit(), 0, 0, SW_SHOWNORMAL );
    } );
#endif
	
#ifdef Q_WS_MAC
	// Use Internet Config to hand the URL to the appropriate application,as
	// set by the user in the Internet Preferences pane.
	// NOTE: ICStart could be called once at Psi startup, saving the
	//       ICInstance in a global variable, as a minor optimization.
	//       ICStop should then be called at Psi shutdown if ICStart succeeded.
	ICInstance icInstance;
	OSType psiSignature = 'Psi ';
	OSStatus error = ::ICStart( &icInstance, psiSignature );
	if ( error == noErr ) {
		ConstStr255Param hint( 0x0 );
		const char* data = url.toAscii();
		long length = url.length();
		long start( 0 );
		long end( length );
		// Don't bother testing return value (error); launched application will
		// report problems.
			::ICLaunchURL( icInstance, hint, data, length, &start, &end );
		ICStop( icInstance );
	}
#endif
}
