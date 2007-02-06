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
	
	$Revision:$
	$Author:$
	$Date:$
	$HeadURL:$

*/

#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H 1

#include <QtCore>
#include <QtGui>
#include <QAssistantClient>

#include "ui_mainwindow.h"
#include "lauewidget.h"
#include "rotatewidget.h"
#include "setubmat.h"
#include "lauecontrols.h"
#include "crystal.h"
#include "crystalwidget.h"
#include "httpcheck.h"

class MainWindowWidget : public QWidget {
	Q_OBJECT
	LaueFilm *film;
	Crystal *crystal;
public:
	MainWindowWidget(QWidget *parent = 0, Crystal *c = 0);
	LaueFilm* getLaueFilm(void)					{ return film; }
	void showMushroom(void);
	void showLaue(void);
protected:
	void resizeEvent(QResizeEvent* event);
};

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
	LaueFilm *film;
	RotateWidget		*rotate;
	LaueControls		*lauecontrols;
	CrystalWidget		*crystalWidget;
	MainWindowWidget	*mainWidget;
	QProgressBar		*progressBar;
	QTableWidget		*indexingWidget;
	QTextEdit			*textViewDockWidget;
	
	// Dock Widget Stuff
	
	QDockWidget *textViewDock;
	QDockWidget *laueControlsDock;
	QDockWidget *mushroomControlsDock;
	QDockWidget *rotateWidgetDock;
	QDockWidget *crystalDock;
	QDockWidget *indexingDock;
	
	// Tool bar stuff
	
	QToolBar *fileToolBar;
	QToolBar *laueToolBar;
	QToolBar *mushroomToolBar;
	QToolBar *crystalToolBar;
	Crystal *crystal;
	
	QString currentCrystalFilename;
	QString currentWorkingDir;
	QPrinter *printer;
	
	HttpCheck *httpcheck;
	
	QAssistantClient *assistantClient;
	
	double currentZoom;
	
	void closeEvent(QCloseEvent *event);
	bool maybeSave(void);
	void writeSettings(void);
	void readSettings(void);
private slots:
	void initializeAssistant(void);
	void about(void);
	void importImage(void);
	void saveCrystal(void);
	void saveAsCrystal(void);
	void open(void);
	void openRecent(void);
	void loadCrystal(QString filename);
	void print(int printmode);
	void printAction(void);
	void pageSetupAction(void);
	void displayUMatrixDialog(void);
	void displayPreferencesDialog(void);
	void displayCrystalDialog(void);
	void updateRecentFileActions();
	void deleteRecentFileActions();
	void setCurrentFile(const QString &fileName);
	void reorient(void);
	QString strippedName(const QString &fullFileName);
	void setDisplayImageControls(bool yesno);
	void zoomIn(void);
	void zoomOut(void);
	void setZoom(double zoom);
	void setZoom100(void) { setZoom(100); }
	void setZoom50(void) { setZoom(50); }
	void setZoom25(void) { setZoom(25); }
	void setZoom10 (void) { setZoom(25); }
	void setZoom200(void) { setZoom(200); }
	void setZoom500(void) { setZoom(500); }
	void setZoom125(void) { setZoom(125); }
	void newCrystal(void);
	void importCif(void);
	void importCel(void);
	void displayOrientation(void);
	void createConnections(void);
	void createDockWindows(void);
	void createToolBars(void);
	void setupCrystal(void);
	void displayCrystalParametersDialog(void);
	void printHtmlPage(QString html);
	void recalculated(void);
	void resetStatusBar(void);
	void setProgressBar(void);
	void rotateCrystal(double newX, double newY, double newZ);
	void rotateCrystalBy(double newX, double newY, double newZ);
	void reCalcAll(void);
	void setGoniometer(void);
	void displayIndexingResults(int nOrientations, LaueOrientations *orientations);
	void displaySingleIndexingResult(int row, int col);
	void displaySingleIndexingResult(void);
	void clearIndexingResults(void);
	void displayHelp(void);
	void displayHelpError(QString message);
public:
	MainWindow();
private:
	Ui::MainWindow ui;
	void setupActions(void);
	enum { MaxRecentFiles = 4 };
	QAction *recentFileActs[MaxRecentFiles];
protected:
	void dropEvent(QDropEvent *event);
};

#endif