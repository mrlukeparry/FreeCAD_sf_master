/***************************************************************************
 *   Copyright (c) 2012 Luke Parry    (l.parry@warwick.ac.uk)              *
 *   Copyright (c) 2012-3 Andrew Robinson <andrewjrobinson@gmail.com>      *
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

#include <Mod/Cam2/App/PreCompiled.h>	// we need the __declspec(dllexport) macros for the Windows build
#include <vector>
#include <QString>
#include <QList>

#include <App/PropertyStandard.h>


namespace Cam
{
class TPGSettingOption;

class CamExport TPGSettingOption
{
public:
    QString id;
    QString value;

    TPGSettingOption(QString id, QString value)
    {
        this->id = id;
        this->value = value;
    }

    TPGSettingOption(const char *id, const char *value)
    {
        this->id = QString::fromUtf8(id);
        this->value = QString::fromUtf8(value);
    }
};

/**
 * A Class object to store the details of a single setting
 */
class CamExport TPGSetting
{
protected:

    int refcnt;

public:
	//(<name>, <label>, <type>, <defaultvalue>, <units>, <helptext>)
	QString name;
	QString label;
	QString type;
	QString defaultvalue;
	QString units;
	QString helptext;
	QList<TPGSettingOption*> options;

	QString value;

	TPGSetting(const char *name, const char *label, const char *type, const char *defaultvalue, const char *units, const char *helptext);
	TPGSetting(QString &name, QString &label, QString &type, QString &defaultvalue, QString &units, QString &helptext);
	TPGSetting();

	~TPGSetting();

	/**
	 * Perform a deep copy of this class
	 */
    TPGSetting* clone();

    void print();

    void setDefault();

    /**
     * Increases reference count
     * Note: it returns a pointer to this for convenience.
     */
    TPGSetting *grab() {
        refcnt++;
        return this;
    }

    /**
     * Decreases reference count and deletes self if no other references
     */
    void release() {
        refcnt--;
        if (refcnt == 0)
            delete this;
    }

    void addOption(QString id, QString value);
    void addOption(const char *id, const char *value);
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

    /**
     * Add a setting to this Settings collection
     */
    TPGSetting* addSetting(TPGSetting* setting);

    /**
     * Get the value of a given setting (by name)
     */
    QString *getSetting(const char *name);

    /**
     * Print the settings to stdout
     */
    void print();

    std::vector<TPGSetting*> getSettings() {
        return this->settings;
    }

    /**
     * Sets the default value for every setting in Settings collection
     */
    void setDefaults();

    /**
     * Increases reference count
     * Note: it returns a pointer to this for convenience.
     */
    TPGSettings *grab() {
        refcnt++;
        return this;
    }

    /**
     * Decreases reference count and deletes self if no other references
     */
    void release() {
        refcnt--;
        if (refcnt == 0)
            delete this;
    }
protected:

    std::vector<TPGSetting*> settings;
    int refcnt;
};

} //namespace Cam


#endif //CAM_TPGSETTINGS_H
