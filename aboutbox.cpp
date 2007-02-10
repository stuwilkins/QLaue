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
#include "aboutbox.h"
#include "version.h"
#include "svn.h"

AboutBox::AboutBox(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);
	ui.ImageLabel->setScaledContents(true);
	ui.ImageLabel->setPixmap(QPixmap(":crystal.png"));
	
	ui.CopyrightNotice->setText(QString("%1 Stuart B. Wilkins 2007").arg(QChar(169)));
	
	QString aboutText("<html><body>");
	
#ifdef SVN_REVISION_TEXT
	aboutText += QString(SVN_REVISION_TEXT).replace("$","") + QString("<br>") 
		+ QString(SVN_AUTHOR_TEXT).replace("$","") + QString("<br>") 
		+ QString(SVN_DATE_TEXT).replace("$","") + QString("<br>");
#endif
	
	QFile infoFile(":info.html");
	if (infoFile.open(QIODevice::ReadOnly | QIODevice::Text)){
		QTextStream in(&infoFile);
		while (!in.atEnd()) {
			aboutText += in.readLine();
			aboutText += "\n";
		}
	} else {
		qDebug("AboutBox::AboutBox() : Unable to open info.html");
	}

	aboutText += "</body></html>";
	ui.TextLabel->setAcceptRichText(true);
	ui.TextLabel->setReadOnly(true);
	ui.TextLabel->setAlignment(Qt::AlignJustify);
	ui.TextLabel->setHtml(aboutText);
}