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

#include <QtCore>
#include <QtXml/QtXml>
#include <QImage>
#include "datafile.h"
#include "crystal.h"
#include "laue.h"
#include "matrix.h"
#include "version.h"
//#include "singlecrystal.h"

DataFile::DataFile(): 
domDocument(APP_NAME) {
	
	experimentDom = domDocument.createElement(APP_NAME);
    domDocument.appendChild(experimentDom);
	QDomComment comment = domDocument.createComment(QString(APP_NAME) + QString(" ") + QString(APP_COPYLEFT));
	experimentDom.appendChild(comment);
	comment = domDocument.createComment(QString("All Rights Reserved"));
	experimentDom.appendChild(comment);
	QDateTime now = QDateTime::currentDateTime();
	comment = domDocument.createComment(QString("Created on ") 
													+ now.toString("ddd MMMM d yy hh:mm:ss"));
	experimentDom.appendChild(comment);
	
	ox = 0; oy = 0;
	
}

bool DataFile::write(QString filename){
	const int IndentSize = 4;
	
	QFile file(filename);
	file.open(QFile::WriteOnly | QFile::Text);
	
	QTextStream out(&file);
	domDocument.save(out, IndentSize);
	return true;
}

bool DataFile::read(QString filename){
	
	QFile file(filename);
	file.open(QFile::ReadOnly | QFile::Text);
	
	if (!domDocument.setContent(&file, true, &errorStr, &errorLine,
								&errorColumn)) {
		return false;
	}
	
	parse();
	crystal.spaceGroupGenerate();
	
	return true;
}

Crystal DataFile::getCrystal(void){
	return crystal;
}

void DataFile::getLaue(LaueFilm* laue){
	laue->getLaue()->setLambdaRange(lambda_min, lambda_max);
	laue->getLaue()->setDet(det_thresh, det_sat);
	laue->getLaue()->setDistance(distance);
}

QImage DataFile::getLaueImage(void){
	return laueImage;
}

void DataFile::parse(void){
	
	QDomNode n = domDocument.firstChild();
    while (!n.isNull()) {
        if (n.isElement()) {
            QDomElement e = n.toElement();
			if(e.tagName() == APP_NAME)
				parseExperiment(n);
			else
				qDebug("DataFile::parse () : unknown token %s",qPrintable(e.tagName()));
            break;
        }
        n = n.nextSibling();
    }
}

void DataFile::parseExperiment(QDomNode n){
	QDomNode experiment = n.firstChild();
	while(!experiment.isNull()){
		if(experiment.isElement()){
			QDomElement e = experiment.toElement();
			if(e.tagName() == "crystal"){
				parseCrystal(experiment);
			}
			if(e.tagName() == "laue"){
				parseLaue(experiment);
			}
		}
		experiment = experiment.nextSibling();
	}
}

void DataFile::parseLaueImage(QDomNode n){
	
	// Delete all information for the crystal
	
	QDomNode l = n.firstChild();
	while(!l.isNull()){
		if(l.isElement()){
			QDomElement e = l.toElement();
			if(e.tagName() == "imagedata"){
				ox = e.attribute("ox","0").toDouble();
				oy = e.attribute("oy","0").toDouble();
				scale = e.attribute("scale","0").toDouble();
				
				QByteArray base64 = e.text().toAscii();
				laueImage.loadFromData(QByteArray::fromBase64(base64));
			}
		}
		l = l.nextSibling();
	}
}

void DataFile::parseLaue(QDomNode n){
	
	// Delete all information for the crystal
	
	QDomNode l = n.firstChild();
	while(!l.isNull()){
		if(l.isElement()){
			QDomElement e = l.toElement();
			if(e.tagName() == "parameters"){
				parseParameters(l);
			}
			if(e.tagName() == "laue-image"){
				parseLaueImage(l);
			}
		}
		l = l.nextSibling();
	}
}

void DataFile::parseOrientation(QDomNode n){
	double ph = 0,pk = 0,pl = 0;
	double sh = 0,sk = 0,sl = 0;
	bool pflag = false;
	bool sflag = false;
	
	QDomNode l = n.firstChild();
	while(!l.isNull()){
		if(l.isElement()){
			QDomElement e = l.toElement();
			if(e.tagName() == "x-axis"){
				pflag = true;
				ph = e.attribute("x","0").toDouble();
				pk = e.attribute("y","0").toDouble();
				pl = e.attribute("z","0").toDouble();
			}
			if(e.tagName() == "z-axis"){
				sflag = true;
				sh = e.attribute("x","0").toDouble();
				sk = e.attribute("y","0").toDouble();
				sl = e.attribute("z","0").toDouble();
			}
		}
		l = l.nextSibling();
	}
	
	if(pflag && sflag)
		crystal.setU(ph,pk,pl,sh,sk,sl);
}

void DataFile::parseParameters(QDomNode n){
	QDomNode l = n.firstChild();
	//cerr << "DataFile::parseParameters()" << endl;
	while(!l.isNull()){
		if(l.isElement()){
			QDomElement e = l.toElement();
			if(e.tagName() == "lambda-range"){
				lambda_min = e.attribute("min","0.3").toDouble();
				lambda_max = e.attribute("max","3").toDouble();
			} else if(e.tagName() == "distance") {
				distance = e.attribute("d","35").toDouble();
			} else if(e.tagName() == "detector") {
				det_thresh = e.attribute("threshold","0.02").toDouble();
				det_sat = e.attribute("saturation","0.75").toDouble();
			}
		}
		l = l.nextSibling();
	}
}

void DataFile::parseCrystal(QDomNode n){
	// Delete all information for the crystal
	
	crystal.delAllAtoms();
	
	QDomNode c = n.firstChild();
	while(!c.isNull()){
		if(c.isElement()){
			QDomElement e = c.toElement();
			if(e.tagName() == "lattice"){
				 crystal.setLattice(e.attribute("a","0").toDouble(),
									e.attribute("b","0").toDouble(),
									e.attribute("c","0").toDouble(),
									M_PI * e.attribute("alpha","0").toDouble() / 180,
									M_PI * e.attribute("beta","0").toDouble() / 180,
									M_PI * e.attribute("gamma","0").toDouble() / 180);
				parseAtoms(c);
			} else if(e.tagName() == "spacegroup"){
				parseSpacegroup(c);
			} else if(e.tagName() == "description"){
				crystal.setName(e.text().toAscii());
			} else if(e.tagName() == "orientation")
				parseOrientation(c);
		}
		c = c.nextSibling();
	}
}

void DataFile::parseAtoms(QDomNode n){
	//qDebug("DataFile::parseAtoms() : parsing atom");
	QDomNode a = n.firstChild();
	while(!a.isNull()){
		if(a.isElement()){
			QDomElement e = a.toElement();
			if(e.tagName() == "atom"){
				//qDebug("DataFile::parseAtoms() : found atom");
				crystal.addAtom(e.attribute("Z","0").toInt(),
								e.attribute("a","0").toDouble(),
								e.attribute("b","0").toDouble(),
								e.attribute("c","0").toDouble());
			} else {
				QString tag = e.tagName();
			}
		}
		a = a.nextSibling();
	}
}

void DataFile::parseSpacegroup(QDomNode n){
	//qDebug("DataFile::parseAtoms() : parsing spacegroup");
	QDomNode sg = n.firstChild();
	while(!sg.isNull()){
		if(sg.isElement()){
			QDomElement e = sg.toElement();
			if(e.tagName() == "hall_name"){
				QString name = e.attribute("name","");
				//qDebug("Setting spacegroup = %s",qPrintable(name));
				if(!name.isEmpty())
					crystal.setSpaceGroup((const char*)name.toAscii());
			}
			if(e.tagName() == "iucname"){
				QString name = e.attribute("name","");
				if(!name.isEmpty()){
					//qDebug("Setting spacegroup = %s",qPrintable(name));
					crystal.setSpaceGroupIUC((const char*)name.toAscii());
				}
			}
		}
		sg = sg.nextSibling();
	}
}

void DataFile::setCrystal(Crystal *crystal){
	
	QDomElement crystalDom = domDocument.createElement("crystal");
	experimentDom.appendChild(crystalDom);
	
	char *desc = crystal->getName();
	if(desc != NULL){
		QDomElement name = domDocument.createElement("description");
		QDomText text = domDocument.createTextNode(QString("%1").arg(desc));
		name.appendChild(text);
		crystalDom.appendChild(name);
	}
	
	QDomElement lattice = domDocument.createElement("lattice");
	lattice.setAttribute("a",crystal->getLatticeA());
	lattice.setAttribute("b",crystal->getLatticeB());
	lattice.setAttribute("c",crystal->getLatticeC());
	lattice.setAttribute("alpha",180 * crystal->getLatticeAlpha() / M_PI);
	lattice.setAttribute("beta",180 * crystal->getLatticeBeta() / M_PI);
	lattice.setAttribute("gamma",180 * crystal->getLatticeGamma() / M_PI);
	
	for(int i=0;i<crystal->getNRootAtoms();i++){
		QDomElement atom = domDocument.createElement("atom");
		atom.setAttribute("a",crystal->getAtom(i)->getXPos());
		atom.setAttribute("b",crystal->getAtom(i)->getYPos());
		atom.setAttribute("c",crystal->getAtom(i)->getZPos());
		atom.setAttribute("Z",crystal->getAtom(i)->getZ());
		lattice.appendChild(atom);
	}
	
    crystalDom.appendChild(lattice);
	
	QDomElement sge = domDocument.createElement("spacegroup");
	QDomElement hallname = domDocument.createElement("hall_name");
	hallname.setAttribute("name",crystal->getSpaceGroup()->HallName);
	sge.appendChild(hallname);
	crystalDom.appendChild(sge);
	
	QDomElement x,y,z;
	x = vectorToElement("x-axis", crystal->xAxis());
	y = vectorToElement("y-axis", crystal->yAxis());
	z = vectorToElement("z-axis", crystal->zAxis());
	QDomElement orientation = domDocument.createElement("orientation");
	orientation.appendChild(x);
	orientation.appendChild(y);
	orientation.appendChild(z);
	crystalDom.appendChild(orientation);
}

void DataFile::setLaue(LaueFilm *film){
	QDomElement l = domDocument.createElement("laue");
	experimentDom.appendChild(l);
	
	QDomElement parameters = domDocument.createElement("parameters");
	
	QDomElement d = domDocument.createElement("distance"); 
	d.setAttribute("d", film->getLaue()->getDistance());
	
	QDomElement lambda = domDocument.createElement("lambda-range");
	lambda.setAttribute("min",film->getLaue()->getLambdaRangeMin());
	lambda.setAttribute("max",film->getLaue()->getLambdaRangeMax());
	
	QDomElement detector = domDocument.createElement("detector");
	detector.setAttribute("saturation",film->getLaue()->getDetSatLevel());
	detector.setAttribute("threshold",film->getLaue()->getDetLevel());
	
	parameters.appendChild(lambda);
	parameters.appendChild(d);
	parameters.appendChild(detector);
	
	l.appendChild(parameters);
	
	if(!film->importedImage->isNull()){
		QByteArray imageOut;
		
		QBuffer buffer(&imageOut);
		buffer.open(QIODevice::WriteOnly);
		film->importedImage->save(&buffer, "PNG");
		
		QDomElement li = domDocument.createElement("laue-image");
		experimentDom.appendChild(l);
		
		QDomElement imagedata = domDocument.createElement("imagedata");
		
		imagedata.setAttribute("ox",film->originX());
		imagedata.setAttribute("oy",film->originY());
		imagedata.setAttribute("scale",film->scale());
		
		QDomText text = domDocument.createTextNode(imageOut.toBase64());
		imagedata.appendChild(text);
		li.appendChild(imagedata);		
		l.appendChild(li);
	}
}

/*
void DataFile::setSingleCrystDiff(SingleCrystalDiff *diff){
	QDomElement l = domDocument.createElement("single-crystal");
	experimentDom.appendChild(l);
	QDomElement parameters = domDocument.createElement("parameters");
	
	QDomElement beam = domDocument.createElement("beam");
	beam.setAttribute("lambda", diff->lambda());
	
	parameters.appendChild(beam);
	l.appendChild(parameters);
}
*/

QDomElement DataFile::vectorToElement(const QString name, const Matrix m){
	QDomElement vector = domDocument.createElement(name);
	vector.setAttribute("x",m.Get(0,0));
	vector.setAttribute("y",m.Get(1,0));
	vector.setAttribute("z",m.Get(2,0));
	return vector;
}

int DataFile::getErrorLine(void){
	return errorLine;
}

int DataFile::getErrorColumn(void){
	return errorColumn;
}

QString DataFile::getErrorStr(void){
	return errorStr;
}

bool DataFile::readCif(QString filename){
	
	QFile file(filename);
	file.open(QFile::ReadOnly | QFile::Text);
	
	double a = 0,b = 0,c = 0;
	double alpha = 0,beta = 0,gamma = 0;
	//int num;
	
	while (!file.atEnd()) {
		QByteArray line = file.readLine();
		QStringList list = QString(line).split(QRegExp("\\s+"),QString::SkipEmptyParts);
		
		if(!list.isEmpty()){
			qDebug("DataFile::process_cif_line() : token == (%s)",qPrintable(list.at(0)));
			process_cif_numvalue(list,"_cell_length_a", &a);
			process_cif_numvalue(list,"_cell_length_b", &b);
			process_cif_numvalue(list,"_cell_length_c", &c);
			process_cif_numvalue(list,"_cell_angle_alpha", &alpha);
			process_cif_numvalue(list,"_cell_angle_beta", &beta);
			process_cif_numvalue(list,"_cell_angle_gamma", &gamma);
			//if(process_cif_numvalue(list,"_symmetry_Int_Tables_number", &num))
			//	crystal.setSpaceGroup(num);
			if(list.at(0) == "_symmetry_space_group_name_H-M")
				cerr << "Spacegroup = " << qPrintable(list.at(1)) << endl;
		}
	}
	
	crystal.setLattice(a,b,c,M_PI * alpha / 180,M_PI * beta / 180,M_PI * gamma / 180);
	crystal.spaceGroupGenerate();
	return true;
}

bool DataFile::process_cif_numvalue(QStringList list, QString tok, double* num){
	if(list.at(0) == tok){
		QString val = list.at(1);
		val.replace('(',"");
		val.replace(')',"");
		*num = val.toDouble();
		return true;
	}
	return false;
}

bool DataFile::process_cif_numvalue(QStringList list, QString tok, int* num){
	if(list.at(0) == tok){
		QString val = list.at(1);
		val.replace('(',"");
		val.replace(')',"");
		*num = val.toInt();
		return true;
	}
	return false;
}

bool DataFile::readCel(QString filename){
	
	QFile file(filename);
	file.open(QFile::ReadOnly | QFile::Text);
	
	double a = 0,b = 0,c = 0;
	double alpha = 0,beta = 0,gamma = 0;
	
	while (!file.atEnd()) {
		QByteArray line = file.readLine();
		QStringList list = QString(line).split(QRegExp("\\s+"),QString::SkipEmptyParts);
		
		if(!list.isEmpty()){
			if(list.at(0) == "CELL"){
				a = list.at(1).toDouble();
				b = list.at(2).toDouble();
				c = list.at(3).toDouble();
				alpha = list.at(4).toDouble();
				beta = list.at(5).toDouble();
				gamma = list.at(6).toDouble();
			} else if (list.at(0) == "natom") {
				crystal.delAllAtoms();
				
				for(int a=0; a< list.at(1).toInt() ; ++a){
					QByteArray line = file.readLine();
					QStringList list = QString(line).split(QRegExp("\\s+"),QString::SkipEmptyParts);
					crystal.addAtom(list.at(1).toInt(),
									list.at(2).toDouble(),
									list.at(3).toDouble(),
									list.at(4).toDouble());
					
				}
			} else if (list.at(0) == "rgnr") {
				// Spacegroup
				crystal.setSpaceGroupNum(list.at(1).toInt());
			} else {
				qDebug("DataFile::readCel() : unknown token %s",qPrintable(list.at(0)));
			}
		}
	}
	
	crystal.setLattice(a,b,c,M_PI * alpha / 180,M_PI * beta / 180,M_PI * gamma / 180);
	crystal.spaceGroupGenerate();
	return true;
}