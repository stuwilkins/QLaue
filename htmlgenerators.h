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

#ifndef _HTMLGENERATORS_H
#define _HTMLGENERATORS_H

#include <QtCore>
#include "laue.h"
//#include "singlecrystal.h"
#include "crystal.h"

class HtmlGenerators {
public:
	enum {
		CrystalBasic = 0,
		CrystalAtomsSgGen = 1,
		CrystalAtoms = 2,
		CrystalAll = 255
	};
	
	static QString crystalToHtml(Crystal *crystal, int mode = 0);
	static QString spacegroupToHtml(Crystal *crystal);
	static QString appHeader(void);
	static QString laueToHtml(lauegram* laue);
	static QString crystalOrientationToHtml(Crystal *crystal);
	static QString crystalOrientationToHtml(Reorientation *reorientation);
/* Used for single crystal portion
	static QString singlecrystaldiffToHtml(SingleCrystalDiff *diff);
	static QString singleCrystalParamsToHtml(SingleCrystalDiff *diff);
	static QString superlatticeToHtml(SingleCrystalDiff *diff);
*/
};

#endif
