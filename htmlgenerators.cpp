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

#include "htmlgenerators.h"
#include "svn.h"
#include "laue.h"
#include "version.h"

//#define APP_VERSION 0.1

QString HtmlGenerators::appHeader(void){
	QString html;
	
	html = "<h2>";
	html += APP_NAME;
	html += "</h2>";
	html += "&#169;</br><b><i>Stuart B. Wilkins 2006, All rights reserved</i><br>";
#ifdef APP_VERSION
	html += QString("Version : %1<br>").arg(APP_VERSION);
#else
	#ifdef SVN_REVISION
		html += SVN_REVISION;
		html += "<br>";
	#endif
#endif
	html += "</b>&nbsp;<br>";
	return html;
}

QString HtmlGenerators::crystalToHtml(Crystal *crystal, int mode) {
	QString chtml;
	
	if(crystal->getName() != NULL){
		chtml += QString("<b>%1</b><br>").arg(crystal->getName()).replace('\n',"<br>");
		chtml += "&nbsp;<br>";
	}
	
	chtml += "<table align=left>\n";
	chtml += "<tr><td width=45\%><b>Direct Space Lattice<td width=15\%><b>a<td width=15\%><b>b<td width=15\%><b>c";
	chtml += QString("<tr><td>&nbsp;<td>%1&Aring;").arg(crystal->getLatticeA(), 5, 'f', 4);
	chtml += QString("<td>%1&Aring;").arg(crystal->getLatticeB(), 5, 'f', 4);
	chtml += QString("<td>%1&Aring;").arg(crystal->getLatticeC(), 5, 'f', 4);
	chtml += "</table>\n";
	
	chtml += "<table>\n";
	chtml += "<tr><td width=45\%>&nbsp;<td width=15\%><b>&#945;<td width=15\%><b>&#946;<td width=15\%><b>&#947;";
	chtml += QString("<tr><td>&nbsp;<td>%1").arg(crystal->getLatticeAlpha() * 180 / M_PI, 5, 'f', 2);
	chtml += QString("<td>%1").arg(crystal->getLatticeBeta() * 180 / M_PI, 5, 'f', 2);
	chtml += QString("<td>%1").arg(crystal->getLatticeGamma() * 180 / M_PI, 5, 'f', 2);
	chtml += "</table>";
	
	// Dump Atoms into two tables
	chtml += "&nbsp;<br><b>Spacegroup : ";
	chtml += QString("%1 %3:%4</b> (Hall Name %2)<br>&nbsp;<br>")
				.arg(crystal->getSpaceGroup()->number)
				.arg(crystal->getSpaceGroup()->HallName)
				.arg(crystal->getSpaceGroup()->Name).replace("_","")
				.arg(crystal->getSpaceGroup()->Extn);
	
	if(mode & CrystalAtoms){
		chtml += "<b>Root Atoms</b><br>\n";
		chtml += "&nbsp;<br>";
		chtml += "<table border=0>\n";
		chtml += "<tr><td width=15\%><b>No.<td width=15\%>&nbsp;<td width=15\%><b>Atom<td width=15\%><b>X<td width=15\%><b>Y<td width=15\%><b>Z\n";
		for(int n=0; n< crystal->getNAtoms() ; n++){
			chtml += "<tr>\n";
			chtml += "<td width=15\%>" + QString("%1").arg(n) + "\n";
			if(crystal->getAtom(n)->rootAtom() == -1)
				chtml += "<td width=15\%>&nbsp;";	
			else
				chtml += "<td width=15\%>" + QString("%1").arg(crystal->getAtom(n)->rootAtom()) + "\n";
			chtml += "<td width=15\%>" + QString("%1").arg(crystal->getAtom(n)->getZName()) + "\n";
			chtml += "<td width=15\%>" + QString("%1").arg(crystal->getAtom(n)->getXPos()) + "\n";
			chtml += "<td width=15\%>" + QString("%1").arg(crystal->getAtom(n)->getYPos()) + "\n";
			chtml += "<td width=15\%>" + QString("%1").arg(crystal->getAtom(n)->getZPos()) + "\n";
			
			if((n == (crystal->getNRootAtoms() - 1)) && (n != 0)){
				if(!(mode & CrystalAtomsSgGen))
					break;
				chtml += "</table>\n";
				chtml += "&nbsp;<br>";
				chtml += "<b>Space Group Generated</b><br>\n";
				chtml += "&nbsp;<br>";
				chtml += "<table border=0>\n";
				chtml += "<tr><td width=15\%><b>No.<td width=15\%><b>Parent<td width=15\%><b>Atom<td width=15\%><b>X<td width=15\%><b>Y<td width=15\%><b>Z\n";
			}
		}
	}
	
	chtml += "</table>";
	
	return chtml;
}

QString HtmlGenerators::spacegroupToHtml(Crystal *crystal) {
	QString html;
	
	QStringList axes;
	
	axes << "x" << "y" << "z";
	
	html = "<b>Spacegroup Generators</b><br>\n";
	
	html += "<table>\n<tr>\n";
	html += "<td width=20\%>&nbsp;";
	html += "<td width=20\%>&nbsp;";
	html += "<td width=20\%>&nbsp;\n";
	
	for(int gen = 0; gen < crystal->getSpaceGroup()->ngenerators; gen++){
		html += "<tr>\n";
		for(int i=0 ; i < 3 ; i++){
			html += "<td>";
		
			for( int j = 0 ; j < 3 ; j++){
				int num = crystal->getSpaceGroup()->matrix[gen][j + (3 * i)];
				if(num){
					if(num < 0){
						html += "-";
					}
				
					html += axes.at(j);
				}
			}
			
			int trans = crystal->getSpaceGroup()->matrix[gen][9 + i];
			if(trans) {
				if(trans < 0) {
					html += " - ";
				} else {
					html += " + ";
				}
				
				int n = abs(trans);
				int d = Crystal::STBF;
				html += QString("<sup>%1/</sup><sub>%2</sub>").arg(n).arg(d);
			}
		}
	}
	
	html += "</table>";
	
	return html;
}

QString HtmlGenerators::laueToHtml(lauegram* laue){
	QString html;
	
	html  = "<table>\n<tr>";
	html += "<td width=40\%>&nbsp;<td width=40\%>&nbsp;";
	
	html += "<tr><td><b>Sample-detector distance:</b><td>";
	html += QString("%1 mm").arg(laue->getDistance());
	html += "<tr><td><b>Detector saturation:</b><td>";
	html += QString("%1\%").arg(100 * laue->detSaturation());
	html += "<tr><td><b>Detector threshhold:</b><td>";
	html += QString("%1\%").arg(100 * laue->detLevel());
	html += "<tr><td><b>Detector threshhold:</b><td>";
	html += QString("%1\%").arg(100 * laue->detLevel());
	html += "<tr><td><b>Lambda max :</b><td>";
	html += QString("%1 &Aring;").arg(laue->getLambdaRangeMax());
	html += "<tr><td><b>Lambda min :</b><td>";
	html += QString("%1 &Aring;").arg(laue->getLambdaRangeMin());
	html += "<tr><td><b>Calculation Accuracy :</b><td>";
	html += QString("%1/10").arg(11 - laue->calcPrecision());
	
	html += "</table>\n";
	return html;
}

QString HtmlGenerators::crystalOrientationToHtml(Crystal *crystal){
	QString html;
	
	html  = "<table>\n<tr>";
	html += "<td width=40\%>&nbsp;<td width=10\%>&nbsp;<td width=10\%>&nbsp;<td width=10\%>&nbsp;";
	
	html += "<tr><td><b>X-Axis (beam axis):</b>";
	Matrix axis(3,1);
	axis = crystal->xAxis();
	html += QString("<td>%1").arg(axis.Get(0,0),5,'f',3);
	html += QString("<td>%1").arg(axis.Get(1,0),5,'f',3);
	html += QString("<td>%1").arg(axis.Get(2,0),5,'f',3);
	
	html += "<tr><td><b>Y-Axis:</b>";
	axis = crystal->yAxis();
	html += QString("<td>%1").arg(axis.Get(0,0),5,'f',3);
	html += QString("<td>%1").arg(axis.Get(1,0),5,'f',3);
	html += QString("<td>%1").arg(axis.Get(2,0),5,'f',3);

	html += "<tr><td><b>Z-Axis (camera axis):</b>";
	axis = crystal->zAxis();
	html += QString("<td>%1").arg(axis.Get(0,0),5,'f',3);
	html += QString("<td>%1").arg(axis.Get(1,0),5,'f',3);
	html += QString("<td>%1").arg(axis.Get(2,0),5,'f',3);
	
	html += "</table>";
	return html;
}

QString HtmlGenerators::crystalOrientationToHtml(Reorientation *reorientation){
	QString html;
	
	html  = "<b>Reorientation to reciprocal space axes:</b><br>";
	html += "<table>\n<tr>";
	html += "<td width=40\%>&nbsp;<td width=10\%>&nbsp;<td width=10\%>&nbsp;<td width=10\%>&nbsp;";
	
	html += "<tr><td><b>X-Axis:</b>";
	Matrix axis(3,1);
	axis = reorientation->getPrimary();;
	html += QString("<td>%1").arg(axis.Get(0,0),5,'f',3);
	html += QString("<td>%1").arg(axis.Get(1,0),5,'f',3);
	html += QString("<td>%1").arg(axis.Get(2,0),5,'f',3);
	
	html += "<tr><td><b>Z-Axis:</b>";
	axis = reorientation->getSecondary();
	html += QString("<td>%1").arg(axis.Get(0,0),5,'f',3);
	html += QString("<td>%1").arg(axis.Get(1,0),5,'f',3);
	html += QString("<td>%1").arg(axis.Get(2,0),5,'f',3);
	
	html += "</table>";
	
	html += "<table>\n<tr>";
	html += "<td width=20\%><b>Rotation X</b><td width=20\%><b>Rotation Y</b><td width=20\%><b>Rotation Z</b>";
	if(reorientation->getMode() == Reorientation::XAxis){
		for(int i=0;i<3;i++){
			html += "<tr>";
			double* angles = reorientation->getAngles(i);
			for(int n=0;n<3;++n){
				html += QString("<td>%1&#176;").arg(angles[n] * 180 / M_PI,5,'f',3, ' ');
			}
		}
	} else {
		html += "<tr>";
		double* angles = reorientation->getAngles(3);
		for(int n=0;n<3;++n){
			html += QString("<td>%1&#176;").arg(angles[n] * 180 / M_PI,5,'f',3, ' ');
		}
	}
	
	html += "</table>";
	
	return html;
}
	