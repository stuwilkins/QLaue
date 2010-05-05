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

#ifndef _DATAFILE_H
#define _DATAFILE_H 1

#include <QtCore>
#include <QtXml/QtXml>
#include "crystal.h"
#include "laue.h"
#include "lauewidget.h"
#include "matrix.h"

class DataFile {
private:
	QDomDocument domDocument;
	QDomElement experimentDom;

	// Last Error 
	
	QString errorStr;
	int errorLine;
	int errorColumn;
	
	// Internal Crystal structure
	
	Crystal crystal;
	
	QImage laueImage;
	
	// Parameters
	
	double ox, oy;
	double scale;
	double lambda_min, lambda_max;
	double det_sat, det_thresh;
	double distance;
	
public:
	DataFile();
	
	// Input / Outpur routines
	
	bool write(QString filename);
	bool read(QString filename);
	bool readCif(QString filename);
	bool readCel(QString filename);
	
	// Error handling
	
	int getErrorLine(void);
	int getErrorColumn(void);
	QString getErrorStr(void);
	
	// Data input 
	
	void setCrystal(Crystal *crystal);
	Crystal getCrystal();
	void setLaue(LaueFilm *film);
	void getLaue(LaueFilm *laue);
	QImage getLaueImage(void);

	double originX(void)		{ return ox; }
	double originY(void)		{ return oy; }
	double imageScale(void)		{ return scale; }
private:
	QDomElement vectorToElement(const QString name, const Matrix m);
	
	// Parsers for data
	
	void parse(void);
	void parseExperiment(QDomNode n);
	void parseCrystal(QDomNode n);
	void parseAtoms(QDomNode n);
	void parseSpacegroup(QDomNode n);
	void parseLaue(QDomNode n);
	void parseOrientation(QDomNode n);
	void parseParameters(QDomNode n);
	void parseLaueImage(QDomNode n);
	// Parsers for non XML data
	
	bool process_cif_numvalue(QStringList list, QString tok, double* num);
	bool process_cif_numvalue(QStringList list, QString tok, int* num);
};

#endif