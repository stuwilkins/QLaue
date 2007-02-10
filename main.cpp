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

#include <QApplication>
#include <QSplashScreen>
#include <QCoreApplication>
#include <QtXml/QtXml>
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "version.h"
#include "svn.h"

int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(resources);	
	
	QApplication app(argc, argv);
	
	QCoreApplication::setOrganizationName("StuartWilkins");
	QCoreApplication::setOrganizationDomain("stuwilkins.com");
	QCoreApplication::setApplicationName(APP_NAME);
	
#ifdef Q_WS_MAC
	//Do nothing
#else
	app.setWindowIcon(QIcon(":crystal.png"));
#endif
	
	QSplashScreen *splash = new QSplashScreen(QPixmap(":splash.png"));
	splash->show();

#ifdef SVN_REVISION_TEXT
	splash->showMessage(QString("Version %1, %2")
		.arg(APP_CURRENT_VERSION).arg(SVN_REVISION_TEXT).replace("$",""), 
		Qt::AlignHCenter | Qt::AlignBottom);
#endif

	MainWindow *mainwindow = new MainWindow;
	
	mainwindow->show();
	
	splash->finish(mainwindow);
	delete splash;
	return app.exec();
}
