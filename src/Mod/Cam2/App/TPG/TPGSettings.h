/***************************************************************************
 *   Copyright (c) 2012 Luke Parry    (l.parry@warwick.ac.uk)              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef CAM_TPGSETTINGS_H
#define CAM_TPGSETTINGS_H

#include <vector>
#include <QString>

#include <App/PropertyStandard.h>


namespace Cam
{

/**
 * A Class object to store the details of a single setting
 */
class CamExport TPGSetting
{
public:
	//(<name>, <label>, <type>, <defaultvalue>, <units>, <helptext>)
	QString name;
	QString label;
	QString type;
	QString defaultvalue;
	QString units;
	QString helptext;

	QString value;

	TPGSetting(const char *name, const char *label, const char *type, const char *defaultvalue, const char *units, const char *helptext);
	TPGSetting(QString &name, QString &label, QString &type, QString &defaultvalue, QString &units, QString &helptext);

	/**
	 * Perform a deep copy of this class
	 */
    TPGSetting* clone();

    void print();
};

  // Class stores hash of settings for managing each independant TPG
class CamExport TPGSettings
{

public:
    TPGSettings();
    ~TPGSettings();

    void initialise() {};
    void loadSettings() {};

	/**
	 * Perform a deep copy of this class
	 */
    TPGSettings* clone();

    void addSetting(TPGSetting* setting);

    /**
     * Print the settings to stdout
     */
    void print();
protected:

    std::vector<TPGSetting*> settings;
};

} //namespace Cam


#endif //CAM_TPGSETTINGS_H
