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

#ifndef _VERSION_H
#define _VERSION_H				1

#define APP_NAME				"QLaue"
#define APP_COPYLEFT			"(c) Stuart Brian Wilkins 2007"
#define APP_CURRENT_VERSION		0.1


#ifdef Q_WS_MAC
	#define APP_VERSION_URL		"http://www.stuwilkins.com:81/QCrystal.macosx"
#endif

#ifdef Q_WS_WIN
	#define APP_VERSION_URL		"http://www.stuwilkins.com:81/QCrystal.win32"
#endif

#ifdef Q_WS_X11
	#define APP_VERSION_URL		"http://www.stuwilkins.com:81/QCrystal.x11"
#endif

#define APP_URL_TO_LOAD			"http://www.stuwilkins.com:81/downloads/QCrystal"

// Now the SVN Headers 

#define SVN_REVISION_TEXT		"$Revision$"
#define SVN_AUTHOR_TEXT			"$Author$"
#define SVN_DATE_TEXT			"$Date$"
#define SVN_HEADURL_TEXT		"$HeadURL$"

#endif
