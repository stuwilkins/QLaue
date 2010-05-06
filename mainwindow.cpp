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

#include <QtGui>
#include <QAbstractItemView>
#ifdef Q_WS_MAC
#include <Carbon/Carbon.h>
#endif
#include "ui_mainwindow.h"
#include "version.h"
#include "mainwindow.h"
#include "lauewidget.h"
#include "rotatewidget.h"
#include "aboutbox.h"
#include "crystaldialog.h"
#include "preferences.h"
#include "setubmat.h"
#include "datafile.h"
#include "reorientdialog.h"
#include "lauecontrols.h"
#include "svn.h"
#include "crystalwidget.h"
#include "htmlviewdialog.h"
#include "htmlgenerators.h"
#include "httpcheck.h"

MainWindowWidget::MainWindowWidget(QWidget *parent, Crystal *c) : QWidget(parent) {
	//setMinimumSize(QSize(300,500));
	crystal = c;
	film = new LaueFilm(this,crystal);
	move(0,0);
}

void MainWindowWidget::resizeEvent(QResizeEvent* event){
	film->resize(event->size());
}

void MainWindowWidget::showLaue(void){ 
	film->show(); 
	film->setFocusPolicy(Qt::StrongFocus);
}

MainWindow::MainWindow() : QMainWindow() {
	ui.setupUi(this);
	
	printer = NULL;
	currentZoom = 100;

	crystal = new Crystal;
	setupCrystal();
	
	mainWidget = new MainWindowWidget(this, crystal);
	film = mainWidget->getLaueFilm();
	
	progressBar = new QProgressBar;
	progressBar->setMaximumSize(QSize(250,100));
	statusBar()->addPermanentWidget(progressBar);
	
	setCentralWidget(mainWidget);
	
	int screenWidth = QApplication::desktop()->width();
	int screenHeight = QApplication::desktop()->height();
	setMaximumSize(screenWidth, screenHeight);
	
	createDockWindows();			// Create Dock Windows
	createConnections();			// Create all connections
	createToolBars();				// Create all Toolbars
	displayOrientation();
	setupActions();					// Setup Menubar
	updateRecentFileActions();		// Setup recent crystals
	
	newCrystal();					// Initializes the crystal
	/*httpcheck = new HttpCheck(this);
	httpcheck->checkVersion();*/
	
	recalculated();
	readSettings();
	
	setCurrentFile(QString(""));
	
	// Initialize help viewer (QT Assistant)
	
	// initializeAssistant();
}

void MainWindow::initializeAssistant() {
/*
  QString assistantDir;
  QString resourcesDir;

#ifdef Q_OS_MACX
	CFURLRef pluginRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
	CFStringRef macPath = CFURLCopyFileSystemPath(pluginRef, 
												  kCFURLPOSIXPathStyle);
	char buffer[1000];
	CFStringGetCString(macPath, buffer, 1000, CFStringGetSystemEncoding());
	assistantDir = QString(buffer);
	assistantDir = assistantDir + QString("/Contents/MacOS");
	resourcesDir = QString(buffer);
	resourcesDir = resourcesDir + QString("/Resources");
	CFRelease(pluginRef);
	CFRelease(macPath);
#endif

	//assistantDir = QString("/usr/local/Trolltech/qt/bin");

	qDebug("MainWindow::initializeAssistant() : Looking for assistant in %s",
		   qPrintable(assistantDir));
	
	assistantClient = new QAssistantClient(assistantDir, this);
	
	QStringList arguments;
	arguments << "-profile" << resourcesDir + QDir::separator() + QString("QLaue.adp");
	assistantClient->setArguments(arguments);
	
	connect(assistantClient, SIGNAL(error(QString)), this, SLOT(displayHelpError(QString)));
  */
}

void MainWindow::displayHelp() {
/*
	QString page("doc/index.html");
	qDebug("MainWindow::displayHelp() : page = %s", qPrintable(page));
	assistantClient->showPage(page);
*/
}

void MainWindow::displayHelpError(QString message) {
	QMessageBox::critical(this,tr(APP_NAME),message);
}

void MainWindow::createDockWindows(void){
	
	rotate = new RotateWidget;
	lauecontrols = new LaueControls;
	
	laueControlsDock = new QDockWidget(tr("Laue Controls"), this);
	laueControlsDock->setObjectName("LaueControlsDock");
	laueControlsDock->setFixedSize(lauecontrols->size() + QSize(0,25));
	laueControlsDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
	laueControlsDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
	laueControlsDock->setWidget(lauecontrols);
	addDockWidget(Qt::LeftDockWidgetArea, laueControlsDock);
		
	rotateWidgetDock = new QDockWidget(tr("Crystal Rotations"), this);
	rotateWidgetDock->setObjectName("RotateWidgetDock");
	rotateWidgetDock->setFixedSize(rotate->size() + QSize(0,25));
	rotateWidgetDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
	rotateWidgetDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
	rotateWidgetDock->setWidget(rotate);
	addDockWidget(Qt::RightDockWidgetArea, rotateWidgetDock);
	rotateWidgetDock->hide();
	
	textViewDock = new QDockWidget(tr("Data View"), this);
	textViewDock->setObjectName("TextViewDock");
	textViewDockWidget = new QTextEdit;
	textViewDockWidget->setReadOnly(false);
	textViewDock->setAllowedAreas(Qt::BottomDockWidgetArea);
	textViewDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
	textViewDock->setWidget(textViewDockWidget);
	addDockWidget(Qt::BottomDockWidgetArea, textViewDock);
	textViewDock->hide();
	
	crystalDock = new QDockWidget(tr("Crystal Orientation"), this);
	crystalDock->setObjectName("CrystalDock");
	crystalWidget = new CrystalWidget(this);
	crystalDock->setWidget(crystalWidget);
	crystalDock->setFixedSize(crystalWidget->size() + QSize(0,25));
	crystalDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
	addDockWidget(Qt::LeftDockWidgetArea,crystalDock);
	crystalDock->show();
	
	indexingDock = new QDockWidget(tr("Indexing Display"), this);
	indexingDock->setObjectName("Indexing");
	indexingDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
	indexingWidget = new QTableWidget(this);
	indexingWidget->show();
	indexingDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
	indexingDock->setWidget(indexingWidget);
	addDockWidget(Qt::RightDockWidgetArea,indexingDock);
	indexingDock->show();
	
	lauecontrols->setAll(film);
}

void MainWindow::createConnections(void){
	
	// Connections for the rotate dialog
	
	connect(rotate, SIGNAL(valueChanged(double,double,double)), this, SLOT(rotateCrystal(double,double,double)));
	connect(rotate, SIGNAL(setGoniometerPos()), this, SLOT(setGoniometer()));
			
	// Allow lauewidget to rotate the crystal
	
	connect(film, SIGNAL(rotationsChanged(double, double, double)), this, SLOT(rotateCrystalBy(double,double,double)));
	
	// Connections for the indexing routines
	
	connect(film->getIndexing(), SIGNAL(newIndexingAvaliable(int, LaueOrientations *)), this, SLOT(displayIndexingResults(int, LaueOrientations *)));
	connect(film->getIndexing(), SIGNAL(calcStarted()), this, SLOT(clearIndexingResults()));
	connect(indexingWidget, SIGNAL(cellClicked(int, int)), this, SLOT(displaySingleIndexingResult(int, int)));
	connect(indexingWidget, SIGNAL(itemSelectionChanged()), this, SLOT(displaySingleIndexingResult()));
	
	// Conections for the LaueControls class
	
	connect(lauecontrols, SIGNAL(sampleFilmDChanged(double)), film, SLOT(setFSDistance(double)));
	connect(lauecontrols, SIGNAL(showSimulation(int)), film, SLOT(setDisplayLaue(int)));
	connect(lauecontrols, SIGNAL(showSimulationIntensities(int)), film, SLOT(setDisplayIntensities(int)));	
	connect(lauecontrols, SIGNAL(detLevelChanged(int)), film, SLOT(setDetLevel(int)));
	connect(lauecontrols, SIGNAL(satLevelChanged(int)), film, SLOT(setSatLevel(int)));
	connect(lauecontrols, SIGNAL(spotSizeChanged(int)), film, SLOT(setSpotSize(int)));
	connect(lauecontrols, SIGNAL(accuracyLevelChanged(int)), film, SLOT(setAccuracy(int)));
	connect(lauecontrols, SIGNAL(showCharacteristicChanged(int)), film, SLOT(setDisplayCharacteristicInt(int)));
	connect(lauecontrols, SIGNAL(showIntensitiesAsColor(bool)), film, SLOT(setDisplayIntensitiesAsColor(bool)));
	
	// Calculation connections
	
	// Connections for progress bar
	
	connect(film->getLaue(),SIGNAL(calcStarted()), this, SLOT(setProgressBar()));
	connect(film->getLaue(),SIGNAL(spotsToGo(int)), progressBar, SLOT(setValue(int)));
	connect(film->getLaue(),SIGNAL(calculated()), this, SLOT(resetStatusBar()));

}

void MainWindow::newCrystal(void){
	setupCrystal();
	
	film->reCalc();
	
	// Remove the laue film
	QImage blank;
	*film->importedImage = blank;
	*film->importedScaledImage = blank;
	setDisplayImageControls(false);
	film->setDisplayImage(false);
	
	// Remove the indexing spots!
	
	film->clearOrientation();
	
}

void MainWindow::setupCrystal(void){
	crystal->setLattice(5.4,5.4,5.4,M_PI/2,M_PI/2,M_PI/2);
	crystal->setSpaceGroup(0); // P_1
	crystal->setU(1,0,0,0,0,1);
	crystal->delAllAtoms();
	crystal->addAtom(29,0,0,0);	
	crystal->setFreeRotate(true);
}

void MainWindow::closeEvent(QCloseEvent *event){
	if (maybeSave()) {
		writeSettings();
		qApp->quit();
	} else {
		event->ignore();
	}
}

bool MainWindow::maybeSave(void){
	return true;
}

void MainWindow::readSettings(void){
	QSettings settings;
	
	int screenWidth = QApplication::desktop()->width();
	int screenHeight = QApplication::desktop()->height();
	
	settings.beginGroup("App");
	double thisVersion = settings.value("version", 0).toDouble();
	qDebug("MainWindow::readSettings() : Last run version %f", thisVersion);
	settings.endGroup();
		
	if(APP_CURRENT_VERSION == thisVersion) {
		qDebug("MainWindow::readSettings() : Version matches last run.");
		settings.beginGroup("MainWindow");
		resize(settings.value("size", QSize(screenWidth*0.9, screenHeight*0.9)).toSize());
		move(settings.value("pos", QPoint(35, 20)).toPoint());
		QByteArray mainstate = settings.value("docks").toByteArray();
	
		if(!mainstate.isEmpty()){
			restoreState(mainstate,1);
		}
		settings.endGroup();
	} else {
		qDebug("MainWindow::readSettings() : Version number does not match last run");
		resize(QSize(screenWidth*0.9, screenHeight*0.9));
		move(QPoint(35, 20));
	}
	
	ui.actionText_Viewer->setChecked(textViewDock->isVisibleTo(this));
	ui.actionCrystal_Orientation_View->setChecked(crystalDock->isVisibleTo(this));
	ui.actionIndexing_Viewer->setChecked(indexingDock->isVisibleTo(this));
	ui.actionRotations_Toolbox->setChecked(rotateWidgetDock->isVisibleTo(this));
	qDebug("MainWindow::readSettings() : rotate = %d", rotateWidgetDock->isVisibleTo(this));
	
}

void MainWindow::writeSettings(void){
	QSettings settings;
	qDebug("MainWindow::writeSettings()");
	
	settings.beginGroup("App");
	settings.setValue("version",(double)APP_CURRENT_VERSION);
	settings.endGroup();
	
	
	settings.beginGroup("MainWindow");
	settings.setValue("pos", pos());
	settings.setValue("size", size());
	settings.setValue("docks", saveState(1));
	settings.endGroup();
	
}

void MainWindow::setupActions(void){

	// Icons for menu and toolbar

	ui.actionNew->setIcon(QIcon(":new.png"));
	ui.actionPrint->setIcon(QIcon(":print.png"));
	ui.action_Open->setIcon(QIcon(":open.png"));
	ui.action_Save->setIcon(QIcon(":filesave.png"));
	ui.actionLaueZoom->setIcon(QIcon(":zoomin.png"));
	ui.actionLaue_Reset_Zoom->setIcon(QIcon(":zoomout.png"));
	ui.actionPreferences->setIcon(QIcon(":config.png"));
	ui.actionView_Crystal_Parameters->setIcon(QIcon(":view.png"));
	ui.actionLattice->setIcon(QIcon(":lattice.png"));
	
	// Laue Menu
	
	ui.actionDefine_Origin->setIcon(QIcon(":center.png"));
	ui.actionMeasure_Scale->setIcon(QIcon(":ruler.png"));
	ui.actionRotate_about_axis->setIcon(QIcon(":rotateabout.png"));
	ui.actionSet_Laue_Orientation->setIcon(QIcon(":ub.png"));
	ui.actionRe_orient->setIcon(QIcon(":gonio.png"));
	ui.actionSolve_Orientation->setIcon(QIcon(":index.png"));
	
	// File menu
	
	connect(ui.actionNew, SIGNAL(triggered()), this, SLOT(newCrystal()));
	connect(ui.actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionPreferences, SIGNAL(triggered()), this, SLOT(displayPreferencesDialog()));
	connect(ui.actionSave_As, SIGNAL(triggered()), this, SLOT(saveAsCrystal()));
	connect(ui.action_Save, SIGNAL(triggered()), this, SLOT(saveCrystal()));
	connect(ui.action_Open, SIGNAL(triggered()), this, SLOT(open()));
	connect(ui.actionPage_Setup, SIGNAL(triggered()), this, SLOT(pageSetupAction()));
	connect(ui.actionPrint, SIGNAL(triggered()), this, SLOT(printAction()));
	connect(ui.actionRecentClear, SIGNAL(triggered()), this, SLOT(deleteRecentFileActions()));
	//connect(ui.actionCIF, SIGNAL(triggered()), this, SLOT(importCif()));
	connect(ui.actionPowder_Cell, SIGNAL(triggered()), this, SLOT(importCel()));

	// Crystal menu
	
	connect(ui.actionSet_Orientation, SIGNAL(triggered()), this, SLOT(displayUMatrixDialog()));
	connect(ui.actionLattice, SIGNAL(triggered()), this, SLOT(displayCrystalDialog()));
	//connect(ui.actionSet_Free_Rotate, SIGNAL(toggled(bool)), rotate, SLOT(setFreeRotate(bool)));
	connect(ui.actionView_Crystal_Parameters, SIGNAL(triggered()), this, SLOT(displayCrystalParametersDialog()));

	// Laue menu
	
	connect(ui.actionImport_Image, SIGNAL(triggered()), this, SLOT(importImage()));
	connect(ui.actionDefine_Origin, SIGNAL(triggered()), film, SLOT(setOrigin()));
	connect(ui.actionMeasure_Scale, SIGNAL(triggered()), film, SLOT(measureScale()));
	connect(ui.actionImage_Scale, SIGNAL(triggered()), film, SLOT(setImageScale()));
	connect(ui.actionReset, SIGNAL(triggered()), film, SLOT(resetView()));
	connect(ui.actionShow_Image, SIGNAL(toggled(bool)), film, SLOT(setDisplayImage(bool)));
	connect(ui.actionInvert_Image, SIGNAL(toggled(bool)), film, SLOT(setInvertImage(bool)));
	connect(ui.actionLaueZoom, SIGNAL(triggered()), film, SLOT(measureZoom()));
	connect(ui.actionLaue_Reset_Zoom, SIGNAL(triggered()), film, SLOT(resetZoom()));
	connect(ui.actionShow_Labels, SIGNAL(toggled(bool)), film, SLOT(showLabels(bool)));
	connect(ui.actionReset_Labels, SIGNAL(triggered()), film, SLOT(resetLabels()));
	connect(ui.actionRe_orient, SIGNAL(triggered()), this, SLOT(reorient()));
	connect(ui.actionSolve_Orientation, SIGNAL(triggered()), film, SLOT(solveOrientation()));
	connect(ui.actionResolve_Orientation, SIGNAL(triggered()), film, SLOT(resolveOrientation()));
	connect(ui.actionSet_Laue_Orientation, SIGNAL(triggered()), film, SLOT(setOrientation()));
	connect(ui.actionRotate_about_axis, SIGNAL(triggered()), film, SLOT(rotateAboutAxis()));
	
	// Window menu
	
	//connect(ui.actionControls_Toolbox, SIGNAL(toggled(bool)), controlsDock, SLOT(setVisible(bool)));
	connect(ui.actionText_Viewer, SIGNAL(toggled(bool)), textViewDock, SLOT(setVisible(bool)));
	connect(ui.actionCrystal_Orientation_View, SIGNAL(toggled(bool)), crystalDock, SLOT(setVisible(bool)));
	connect(ui.actionIndexing_Viewer, SIGNAL(toggled(bool)), indexingDock, SLOT(setVisible(bool)));
	connect(ui.actionRotations_Toolbox, SIGNAL(toggled(bool)), rotateWidgetDock, SLOT(setVisible(bool)));
	
	// Help Menu
	
	connect(ui.action_About, SIGNAL(triggered()), this, SLOT(about()));
	connect(ui.action_Help, SIGNAL(triggered()), this, SLOT(displayHelp()));
	
	setDisplayImageControls(false);
	
	// Initialize the recent file menu.
	
	ui.actionQuitSep->setSeparator(true);
	ui.actionRecentSep->setSeparator(true);
	ui.actionRecentSep->setVisible(false);
	ui.actionRecentClear->setVisible(false);
	
	recentFileActs[0] = ui.actionRecent1;
	recentFileActs[1] = ui.actionRecent2;
	recentFileActs[2] = ui.actionRecent3;
	recentFileActs[3] = ui.actionRecent4;
	
	for (int i = 0; i < MaxRecentFiles; ++i) {
		recentFileActs[i]->setVisible(false);
		connect(recentFileActs[i], SIGNAL(triggered()),
				this, SLOT(openRecent()));
	}

	// Set the defaults
	
	// For macos systems remove the separator before Quit in the file menu.
	
#ifdef Q_WS_MAC
	ui.actionQuitSep->setVisible(false);
#else
	ui.actionQuitSep->setText("");
	ui.actionQuitSep->setVisible(true);
#endif
}

void MainWindow::createToolBars(void){
	fileToolBar = addToolBar(tr("File"));
	fileToolBar->setMovable(false);
	fileToolBar->setObjectName("FileToolBar");
	fileToolBar->addAction(ui.actionNew);
	fileToolBar->addAction(ui.action_Open);
	fileToolBar->addAction(ui.action_Save);
	fileToolBar->addAction(ui.actionPrint);
	fileToolBar->addAction(ui.actionPreferences);
	
	crystalToolBar = addToolBar(tr("Crystal"));
	crystalToolBar->setMovable(false);
	crystalToolBar->setObjectName("CrystalToolBar");
	crystalToolBar->addAction(ui.actionLattice);
	crystalToolBar->addAction(ui.actionView_Crystal_Parameters);
	
	laueToolBar = addToolBar(tr("Laue"));
	laueToolBar->setMovable(false);
	laueToolBar->setObjectName("LaueToolBar");
	//laueToolBar->addAction(ui.actionLaueZoom);
	//laueToolBar->addAction(ui.actionLaue_Reset_Zoom);
	laueToolBar->addAction(ui.actionDefine_Origin);
	laueToolBar->addAction(ui.actionMeasure_Scale);
	laueToolBar->addAction(ui.actionRotate_about_axis);
	laueToolBar->addAction(ui.actionSet_Laue_Orientation);
	laueToolBar->addAction(ui.actionSolve_Orientation);
	laueToolBar->addAction(ui.actionRe_orient);
}

void MainWindow::updateRecentFileActions() {
	QSettings settings;
	QStringList files = settings.value("recentFileList").toStringList();
	
	int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);
	
	if(numRecentFiles == 0){
		
		ui.actionRecentSep->setVisible(false);
		ui.actionRecentClear->setVisible(false);
		recentFileActs[0]->setText("Empty");
		recentFileActs[0]->setVisible(true);
		recentFileActs[0]->setEnabled(false);
	
	} else {
	
		for (int i = 0; i < numRecentFiles; ++i) {
			QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
			recentFileActs[i]->setText(text);
			recentFileActs[i]->setData(files[i]);
			recentFileActs[i]->setVisible(true);
			recentFileActs[i]->setEnabled(true);
		}
		for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
			recentFileActs[j]->setVisible(false);
	
		ui.actionRecentSep->setVisible(true);
		ui.actionRecentClear->setVisible(true);
	}
}

void MainWindow::deleteRecentFileActions() {
	QSettings settings;
	QStringList files;
	settings.setValue("recentFileList", files);
	updateRecentFileActions();
}

void MainWindow::setCurrentFile(const QString &fileName)
{
	currentCrystalFilename = fileName;
	if (currentCrystalFilename.isEmpty())
		setWindowTitle(tr("%1 Version %2").arg(APP_NAME).arg(APP_CURRENT_VERSION));
	else
		setWindowTitle(tr("%2 - [*]%1").arg(strippedName(currentCrystalFilename))
					   .arg(tr(APP_NAME)));
	
	QSettings settings;
	QStringList files = settings.value("recentFileList").toStringList();
	files.removeAll(fileName);
	files.prepend(fileName);
	while (files.size() > MaxRecentFiles)
		files.removeLast();
	
	settings.setValue("recentFileList", files);
	
	foreach (QWidget *widget, QApplication::topLevelWidgets()) {
		MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
		if (mainWin)
			mainWin->updateRecentFileActions();
	}
}

void MainWindow::displayCrystalDialog(void){
	CrystalDialog dialog;
	dialog.setCrystal(crystal);
	if(dialog.exec() == QDialog::Accepted){
		dialog.getCrystal(crystal);
		reCalcAll();
	}
}

void MainWindow::displayPreferencesDialog(void){

	PreferencesDialog dialog;
	dialog.setLaue(film->getLaue());
	
	if(dialog.exec() == QDialog::Accepted){
		dialog.getLaue(film->getLaue());
		reCalcAll();
	}

}

void MainWindow::displayUMatrixDialog(void){
	UMatrixDialog dialog;
	if(dialog.exec() == QDialog::Accepted){
		crystal->setU(dialog.getPrimary(), dialog.getSecondary());
		reCalcAll();
	}
}

void MainWindow::openRecent(void){
	QAction *action = qobject_cast<QAction *>(sender());
	if (action){
		loadCrystal(action->data().toString());
	}
}

void MainWindow::loadCrystal(QString filename){	
	
	DataFile datafile;
	
	if(!datafile.read(filename)){
		// We have an error ... read the errors from 
		// the datafile class
		
		QMessageBox::information(window(), tr(APP_NAME),
								 tr("Parse error at line %1, column %2:\n%3")
								 .arg(datafile.getErrorLine())
								 .arg(datafile.getErrorColumn())
								 .arg(datafile.getErrorStr()));
		return;
	}

	*crystal = datafile.getCrystal();
	datafile.getLaue(film);
	reCalcAll();
	
	QImage image = datafile.getLaueImage();
	if(!image.isNull()){
		*(film->importedImage) = image;
		*(film->importedScaledImage) = image;
		film->setOriginX(datafile.originX());
		film->setOriginY(datafile.originY());
		film->setScale(datafile.imageScale());
		film->setDisplayImage(true);
		setDisplayImageControls(true);
	}
	
	statusBar()->showMessage(QString("Loaded %1")
							 .arg(strippedName(filename)), 5000);
	setCurrentFile(filename);
	lauecontrols->setAll(film);
}

void MainWindow::importCif(void){	
	
	if(!maybeSave())
		return;
	QString filename = QFileDialog::getOpenFileName(this,
											 "Choose a file to import",
											 "",
											 "CIF Files (*.cif)");
	if(filename == NULL)
		return;
	
	DataFile datafile;
	
	if(!datafile.readCif(filename)){
		QMessageBox::information(window(), tr(APP_NAME),
								 tr("Parse error"));
		return;
	}
	
	*crystal = datafile.getCrystal();
	
	statusBar()->showMessage(QString("Imported %1")
							 .arg(strippedName(filename)),5000);
}

void MainWindow::importCel(void){	
	
	if(!maybeSave())
		return;
	QString filename = QFileDialog::getOpenFileName(this,
													"Choose a file to import",
													"",
													"CEL Files (*.cel)");
	if(filename == NULL)
		return;
	
	DataFile datafile;
	
	if(!datafile.readCel(filename)){
		QMessageBox::information(window(), tr(APP_NAME),
								 tr("Parse error"));
		return;
	}
	
	*crystal = datafile.getCrystal();
	reCalcAll();
	statusBar()->showMessage(QString("Imported %1")
							 .arg(strippedName(filename)),5000);
}

void MainWindow::open(void){
	
	if(!maybeSave())
		return;
	
	QString s = QFileDialog::getOpenFileName(this,
											 tr("Choose a file to laod"),
											 currentWorkingDir,
											 tr("Crystals (*.xtl)"));
	if(s == NULL)
		return;
	
	loadCrystal(s);
	setCurrentFile(s);
}

void MainWindow::saveAsCrystal(void){
	QString fileName = QFileDialog::getSaveFileName(this,
											 tr("Choose a file to save under"),
											 currentWorkingDir,
											 tr("Crystals (*.xtl)"));
	if(!fileName.isEmpty()) {
		if(fileName.right(4) != ".xtl") 
			fileName = fileName + ".xtl";
		setCurrentFile(fileName);
		saveCrystal();
	}
}

void MainWindow::saveCrystal(void){
	if(currentCrystalFilename.isEmpty()){
		saveAsCrystal();
		return;
	}
	
	DataFile datafile;
	datafile.setCrystal(crystal);
	datafile.setLaue(film);
	if(!datafile.write(currentCrystalFilename)){
		currentCrystalFilename = "";
	}
	
	statusBar()->showMessage(QString("Saved %1.")
							 .arg(strippedName(currentCrystalFilename)),5000);
	return;
}

void MainWindow::importImage(void){
	QString s = QFileDialog::getOpenFileName(this,
											 "Choose Image",
											 currentWorkingDir,
											 "Images (*.png *.xpm *.jpg *.tif *.tiff)");
	if(s == NULL){
		setDisplayImageControls(false);
		film->setDisplayImage(false);
		return;
	}
	
	QImage pixmap;
	
	if(pixmap.load(s) != TRUE){
		// Could not import image 
		QMessageBox::warning(this,
							 "Import Image",
							 "There was an error in importing the image",
							 QMessageBox::Ok,
							 QMessageBox::NoButton,
							 QMessageBox::NoButton);
		setDisplayImageControls(false);
		film->setDisplayImage(false);
		return;
	}
	
	*(film->importedImage) = pixmap;
	*(film->importedScaledImage) = pixmap;
	//film->setPixelsPerMMFromImage();
	film->setDisplayImage(true);
	setDisplayImageControls(true);
	
	statusBar()->showMessage(QString("Imported %1 (%2 x %3 pixels).")
							 .arg(strippedName(s)).arg(pixmap.width()).arg(pixmap.height()),5000);
}

void MainWindow::printAction(void){
	print(LaueFilm::NoPrintReorientation);
}

void MainWindow::print(int printmode){
	if(!printer)
		printer = new QPrinter(QPrinter::HighResolution);
	
	printer->setResolution(300);
	printer->setOrientation(QPrinter::Landscape);
	QPrintDialog printDialog(printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
		QString printer_name = printer->printerName();
		qDebug("Printing to : %s",qPrintable(printer_name));
		QPainter painter;
		if(!painter.begin(printer)){
			QMessageBox::critical(this, tr(APP_NAME), tr("Printer Error"));
			return;
		}
		printer->setOrientation(QPrinter::Landscape);
		film->print(printer, &painter, printmode);
		painter.end();
    }
	
	delete printer; printer = NULL;
}

void MainWindow::printHtmlPage(QString html){
	if(!printer)
		printer = new QPrinter(QPrinter::ScreenResolution);
	
	//printer->setResolution(300);
	printer->setOrientation(QPrinter::Portrait);
	QPrintDialog printDialog(printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
		QTextDocument textbox;
		QFont serifFont("Times", 6, QFont::Bold);
		textbox.setDefaultFont(serifFont);
		textbox.setHtml(html);
		textbox.print(printer);
	}
	
	delete printer; printer = NULL;
}

void MainWindow::pageSetupAction(void){
	if(!printer){
		printer = new QPrinter(QPrinter::HighResolution);
		printer->setOrientation(QPrinter::Landscape);
	}
	QPageSetupDialog pageSetupDialog(printer, this);
	pageSetupDialog.exec();
}

void MainWindow::about(void){
	AboutBox about;
	about.exec();
}

void MainWindow::reorient(void){
	double angleX, angleY, angleZ;
	int state;
	
	if(!crystal->getFreeRotate()){
		QMessageBox::information(window(), tr(APP_NAME),
			tr("You cannot re-orientate your sample when set to goniometer rotations. Please select \"free rotate\" and try again."));
		return;
	}
	
	ReorientDialog dialog;
	dialog.setCrystal(crystal);
	dialog.setLaue(film->getLaue());
	state = dialog.exec();
	
	if(state == ReorientDialog::OKClicked){
		if(dialog.getSelectedOrientation()){
			dialog.getAngles(&angleX, &angleY, &angleZ);
			rotateCrystal(angleX, angleY, angleZ);
		}
	} else if(state == ReorientDialog::PrintClicked) {
		print(LaueFilm::PrintReorientation);
	}
}

QString MainWindow::strippedName(const QString &fullFileName) {
	return QFileInfo(fullFileName).fileName();
}

void MainWindow::setDisplayImageControls(bool yesno){
	ui.actionInvert_Image->setEnabled(yesno);
	ui.actionShow_Image->setEnabled(yesno);
	ui.actionLaueZoom->setEnabled(yesno);
	ui.actionLaue_Reset_Zoom->setEnabled(yesno);
	//ui.actionLaueZoom->setEnabled(false);
	//ui.actionLaue_Reset_Zoom->setEnabled(false);
}

void MainWindow::zoomIn(void){
	currentZoom += 5;
	if(currentZoom > 500)
		currentZoom = 500;
	film->setZoom(currentZoom);
}

void MainWindow::zoomOut(void){
	currentZoom -= 5;
	if(currentZoom < 20)
		currentZoom = 20;
	film->setZoom(currentZoom);
}

void MainWindow::setZoom(double zoom){
	qDebug("Zoom = %f", zoom);
	currentZoom = zoom;
	film->setZoom(currentZoom);
}

void MainWindow::dropEvent(QDropEvent *event) {
	qDebug("Drop event : %s", qPrintable(event->mimeData()->text()));
}

void MainWindow::displayOrientation(void){

	crystalWidget->displayOrientation(crystal->xAxis(), 
									  crystal->yAxis(),
									  crystal->zAxis());
	
}

void MainWindow::displayCrystalParametersDialog(void){
	HtmlViewDialog dialog;
	dialog.setCrystal(crystal);
	if(dialog.exec() == QDialog::Accepted)
		printHtmlPage(dialog.getHtml());
}

void MainWindow::recalculated(void) {
	//textViewDockWidget->setHtml(HtmlGenerators::singlecrystaldiffToHtml(mushroom->getDiff()));
}

void MainWindow::resetStatusBar(void){
	statusBar()->showMessage(tr("Ready."),5000);
	progressBar->reset();
}

void MainWindow::setProgressBar(void){
	statusBar()->showMessage(tr("Calculating ......"));
}

void MainWindow::rotateCrystalBy(double newX, double newY, double newZ){
	crystal->rotateBy(newX, newY, newZ);
	reCalcAll();
}

void MainWindow::rotateCrystal(double newX, double newY, double newZ){
	crystal->rotateTo(newX, newY, newZ);
	reCalcAll();
}

void MainWindow::reCalcAll(void){
	film->reCalc();
	displayOrientation();
	//qDebug("MainWindow::reCalcAll()");
}

// Routines for the Indexing Widget

void MainWindow::displaySingleIndexingResult(int row, int col){
	Q_UNUSED(col);
	film->setU(film->getIndexing()->getOrientations(row).getU());
}

void MainWindow::displaySingleIndexingResult(void){
	int row = indexingWidget->currentRow();
	film->setU(film->getIndexing()->getOrientations(row).getU());
}

void MainWindow::displayIndexingResults(int nOrientations, LaueOrientations *orientations){
	
	indexingDock->setVisible(true);
	
	// Start some variables
	
	QStringList headerLabels;
	
	headerLabels << "Primary Rfln." << "Secondary Rfln." << "Hits" << "Pattern Diff.";
	
	// Ok so we need to populate the table with the
	// clacc LaueOrientations
	
	indexingWidget->setColumnCount(4);
	indexingWidget->setRowCount(nOrientations);
	indexingWidget->setHorizontalHeaderLabels(headerLabels);
	//indexingWidget->setShowGrid(false);
	indexingWidget->setSortingEnabled(false);
	indexingWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	
	indexingWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	indexingWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	
	for(int i = 0 ; i < nOrientations ; i++){
		QTableWidgetItem *newItem = new QTableWidgetItem(tr("(%1 %2 %3)")
			.arg(orientations[i].getP().Get(0,0))
			.arg(orientations[i].getP().Get(1,0))
			.arg(orientations[i].getP().Get(2,0)));
		newItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		indexingWidget->setItem(i, 0, newItem);
		
		QTableWidgetItem *newItem1 = new QTableWidgetItem(tr("(%1 %2 %3)")
			.arg(orientations[i].getS().Get(0,0))
			.arg(orientations[i].getS().Get(1,0))
			.arg(orientations[i].getS().Get(2,0)));
		newItem1->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		newItem1->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		indexingWidget->setItem(i, 1, newItem1);

		QTableWidgetItem *newItem2 = new QTableWidgetItem(tr("%1").arg(orientations[i].getNumMatches()));
		newItem2->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		newItem2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		indexingWidget->setItem(i, 2, newItem2);
		
		QTableWidgetItem *newItem3 = new QTableWidgetItem(tr("%1").arg(orientations[i].getPatternError()));
		newItem3->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		newItem3->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		indexingWidget->setItem(i, 3, newItem3);
	}
	
	indexingWidget->setCurrentCell(0,0);
}

void MainWindow::clearIndexingResults(void) {
	indexingWidget->clear();
	indexingWidget->setColumnCount(0);
	indexingWidget->setRowCount(0);
}

