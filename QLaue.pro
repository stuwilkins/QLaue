#
#
#	QLaue - Orientation of single crystal samples by the Laue method. 
#	Copyright (C) 2007 Stuart Brian Wilkins
#
#	This program is free software; you can redistribute it and/or
#	modify it under the terms of the GNU General Public License
#	as published by the Free Software Foundation; either version 2
#	of the License, or (at your option) any later version.
#
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#	
#	You should have received a copy of the GNU General Public License
#	along with this program; if not, write to the Free Software
#	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#	
#	$Revision$
#	$Author$
#	$Date$
#	$HeadURL$
#


TEMPLATE = app
TARGET = 
CONFIG += x86_64 

QT += xml network

DEPENDPATH += . ui
INCLUDEPATH += . ui

DEFINES += CALC_IN_THREAD

ICON = mac/icon.icns
RC_FILE = QLaue.rc
RESOURCES = resources.qrc
QMAKE_INFO_PLIST = mac/mac.plist

MOC_DIR = moc
UI_DIR = ui
RCC_DIR = res


# Input
HEADERS += aboutbox.h \
           atom.h \
           crystal.h \
           crystaldialog.h \
           crystalwidget.h \
           datafile.h \
           filmsizedialog.h \
           htmlgenerators.h \
           htmlviewdialog.h \
#           httpcheck.h \
           indexingdialog.h \
           laue.h \
           lauecontrols.h \
           lauewidget.h \
           mainwindow.h \
           matrix.h \
           reorientdialog.h \
           rotatewidget.h \
           setubmat.h \
           sfact_f0.h \
           sfact_f1f2.h \
           spacegroup.h \
           version.h \
           xray.h \
           xrayam.h \
           xraybe.h \
           xrayel.h \
           preferences.h \
	   adjustimage.h
FORMS += aboutbox.ui \
         crystaldialog.ui \
         crystalwidget.ui \
         filmsizedialog.ui \
         htmlviewdialog.ui \
#         httpcheck.ui \
         indexingdialog.ui \
         lauecontrols.ui \
         mainwindow.ui \
         reorientdialog.ui \
         rotatewidget.ui \
         setubmat.ui\
         preferences.ui 
SOURCES += aboutbox.cpp \
           atom.cpp \
           crystal.cpp \
           crystaldialog.cpp \
           crystalwidget.cpp \
           datafile.cpp \
           filmsizedialog.cpp \
           htmlgenerators.cpp \
           htmlviewdialog.cpp \
#           httpcheck.cpp \
           indexingdialog.cpp \
           laue.cpp \
           lauecontrols.cpp \
           lauewidget.cpp \
           main.cpp \
           mainwindow.cpp \
           matrix.cpp \
           reorientdialog.cpp \
           rotatewidget.cpp \
           setubmat.cpp \
           xray.cpp \
           preferences.cpp \
	   adjustimage.cpp 
RESOURCES += resources.qrc
