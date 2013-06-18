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

#include <PreCompiled.h>	// we need the __declspec(dllexport) macros for the Windows build

namespace Cam {
class CamExport TPGSettingOption;
class CamExport TPGSettingDefinition;
class CamExport TPGSettings;
}

#include "../Features/TPGFeature.h"

#include <vector>
#include <QString>
#include <QList>

#include <App/PropertyStandard.h>


namespace Cam
{


class CamExport TPGSettingOption
{
public:
    QString id;
    QString label;

    TPGSettingOption(QString id, QString label)
    {
        this->id = id;
        this->label = label;
    }

    TPGSettingOption(const char *id, const char *label)
    {
        this->id = QString::fromUtf8(id);
        this->label = QString::fromUtf8(label);
    }
};

/**
 * A Class object to store the details of a single setting
 */
class CamExport TPGSettingDefinition
{
protected:

	/// reference counter
    int refcnt;

    /// reference to the settings object that this setting belongs too
    TPGSettings *parent;

    /// the action that this setting belongs to
    QString action;

public:
    friend class TPGSettings;

	//(<name>, <label>, <type>, <defaultvalue>, <units>, <helptext>)
	QString name;
	QString label;
	QString type;
	QString defaultvalue;
	QString units;
	QString helptext;
	QList<TPGSettingOption*> options;

//	QString value; // deprecated: now uses FreeCAD data structure which is contained in TPGSettings.tpgFeature

	TPGSettingDefinition(const char *name, const char *label, const char *type, const char *defaultvalue, const char *units, const char *helptext);
	TPGSettingDefinition(QString &name, QString &label, QString &type, QString &defaultvalue, QString &units, QString &helptext);
	TPGSettingDefinition();

	~TPGSettingDefinition();

	/**
	 * Perform a deep copy of this class
	 */
    TPGSettingDefinition* clone();

    void print();

    /**
     * Increases reference count
     * Note: it returns a pointer to this for convenience.
     */
    TPGSettingDefinition *grab();

    /**
     * Decreases reference count and deletes self if no other references
     */
    void release();

    /// add an option for the value of this setting
    void addOption(QString id, QString label);
    /// add an option for the value of this setting
    void addOption(const char *id, const char *label);

    /// get the value associated with this setting
    QString getValue();

    /// set the value associated with this setting
    bool setValue(QString &value);

    /// get the namespaced name <action>::<name>
    QString getFullname();
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
    TPGSettingDefinition* addSettingDefinition(QString &action, TPGSettingDefinition* setting);

    /**
     * Get the currently selected action.
     */
    const QString &getAction() const;

    /**
     * Change the currently selected action.
     */
    bool setAction(QString &action);

    /**
     * Get the value of a given setting (by name)
     */
    const QString getValue(QString &name);

    /**
     * Get the value of a given setting (by name)
     */
    const QString getValue(QString &action, QString &name);

    /**
     * Print the settings to stdout
     */
    void print();

    std::vector<TPGSettingDefinition*> getSettings();

    /**
     * Sets the default value for every setting in Settings collection
     */
    void addDefaults();

    /**
     * Set the value for the named setting
     */
    bool setValue(QString &action, QString &name, QString &value);

    /**
     * Set the value for the named setting
     */
    bool setValue(QString &name, QString &value);

    /**
     * Sets the TPGFeature that the value will be saved-to/read-from.
     */
    void setTPGFeature(TPGFeature *tpgFeature);

    /**
     * Increases reference count
     * Note: it returns a pointer to this for convenience.
     */
    TPGSettings *grab();

    /**
     * Decreases reference count and deletes self if no other references
     */
    void release();
protected:

    /// the action that is selected (i.e. algorithm and group of settings to use)
    QString action;
    /// the settings in the order that the TPG developer wants them to display
    std::vector<TPGSettingDefinition*> settingDefs;
    /// the same settings but in map data structure for random access (key: <action>::<name>)
    std::map<QString, TPGSettingDefinition*> settingDefsMap;
    /// reference counter
    int refcnt;
    /// the tpgFeature to which these settings belong
    TPGFeature *tpgFeature;

    /// make a namespaced name (from <action>::<name>)
    QString makeName(QString &action, QString &name) const;
};

} //namespace Cam


#endif //CAM_TPGSETTINGS_H
