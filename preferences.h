/*
 *  lauepreferences.h
 *  
 *
 *  Created by Stuart Wilkins on 30/03/2006.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 *  $Id: preferences.h 61 2006-04-28 05:51:43Z svnrepos $
 *  $HeadURL: https://wilkins@dilbert.stuartwilkins.org/repos/QCrystal/trunk/preferences.h $
 *
 */

#ifndef _PREFERENCES_H
#define _PREFERENCES_H 1

#include <QtGui>
#include "laue.h"
#include "ui_preferences.h"

class PreferencesDialog : public QDialog
{
	Q_OBJECT
	
	QStringList anodes;
public:
	PreferencesDialog();
	void setLaue(lauegram* laue);
	void getLaue(lauegram* laue);
	
	enum {
		LaueTab = 0,
		SingleCrystalTab = 1
	};
private:
	Ui::PreferencesDialog ui;
private slots:
	void tubeVoltageChanged(double newval);	
};

#endif