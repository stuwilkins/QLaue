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
#include "htmlviewdialog.h"
#include "htmlgenerators.h"
#include "crystal.h"

HtmlViewDialog::HtmlViewDialog(QWidget* parent) :
QDialog(parent)
{
	ui.setupUi(this);
	ui.textEdit->setReadOnly(true);
}

void HtmlViewDialog::setCrystal(Crystal* crystal){
	QString chtml;
	
	chtml += "<body><html>\n";
	
	chtml += HtmlGenerators::appHeader();
	
	// Dump Lattice parameters
	
	chtml += HtmlGenerators::crystalToHtml(crystal, HtmlGenerators::CrystalAll);
	chtml += "&nbsp;<br>&nbsp;<br>\n";
	chtml += HtmlGenerators::spacegroupToHtml(crystal);
	chtml += "</body></html>\n";
	
	//qDebug("HTML :\n%s",qPrintable(chtml));
	ui.textEdit->setHtml(chtml);
}

QString HtmlViewDialog::getHtml(void) {
	return ui.textEdit->toHtml();
}