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

#include <App/PropertyLinks.h>
#include <App/PropertyStandard.h>

namespace Cam {
	namespace Settings {
	class CamExport Option;
	class CamExport Definition;
	class CamExport TPGSettings;
	}
}

#include "../Features/TPGFeature.h"

#include <vector>
#include <QString>
#include <QList>
#include <QColor>

#include <App/PropertyStandard.h>

namespace Cam
{
	namespace Settings 
	{
class CamExport TPGFeature; //TODO: work out why this is needed (must be some crazy cyclic including)

class CamExport Option
{
public:
    QString id;
    QString label;

    Option(QString id, QString label)
    {
        this->id = id;
        this->label = label;
    }

    Option(const char *id, const char *label)
    {
        this->id = QString::fromUtf8(id);
        this->label = QString::fromUtf8(label);
    }
};

/**
 * A Class object to store the details of a single setting
 */
class CamExport Definition
{
public:
	typedef enum
	{
		Metric = 0,
		Imperial
	} Units_t;

public:
	/**
		Define an enumeration for the types of settings we support.

		NOTE: If any values are added/removed from this enumeration then
		their corresponding ASCII equivalents (used via the Python interface)
		must also be updated.  Such values are defined within the
		PyDefinition_init() method of the PyTPGSettings.cpp file.

		NOTE: The SettingType_ObjectNamesForType setting type assumes that
		the Options list has been populated with the following entries;
			- id = "Delimiters" (as a keyword) and label = <all characters that delimit separate values in the setting>
			- id = "TypeId" (the keyword) and label = <a single class type ID as would be used within the Base::Type::fromName() method>

		There must be only one option with an option.id="Delimiters" but there may be many options with an id="TypeId".

		NOTE: For SettingType_Enumeration, the 'value' is always going to be the integer form.  When it's presented
		in the QComboBox (i.e. the user interface) the verbose (string) form is always used but, once that interaction
		is complete, the value written to the Definition object will always be the integer form.
		To this end, settings of this type must have options whose ID is the integer form and whose LABEL is the string form.
		Since Python is all string-based it might make sense to just use the string form for the value too.  The problem with
		this is that it precludes language changes.  If we use the integer form for the value then the files should carry
		between languages better.
	 */
	typedef enum
	{
		SettingType_Text = 0,	// Values of this type are stored in TPGFeature::PropTPGSettings
		SettingType_Radio,		// Values of this type are stored in TPGFeature::PropTPGSettings
		SettingType_ObjectNamesForType,	// Object names whose types are included in the list of options.
		SettingType_Enumeration,	// Produces a combo-box whose values include the verbose forms of the enumerated type.
		SettingType_Length,			// MUST have units of 'mm' or 'inch' for this to make sense.
		SettingType_Filename,
		SettingType_Directory,
		SettingType_Color,
		SettingType_Integer,
		SettingType_Double
	} SettingType;

	typedef enum {
        Invalid,
        Intermediate,
        Acceptable
	} ValidationState;

protected:

	/// reference counter
    int refcnt;

    /// reference to the settings object that this setting belongs too
    TPGSettings *parent;

    /// the action that this setting belongs to
    QString action;

public:
    friend class TPGSettings;

	bool operator== ( const Definition & rhs ) const;
	

	//(<name>, <label>, <type>, <defaultvalue>, <units>, <helptext>)
	QString name;
	QString label;
	SettingType type;
	QString defaultvalue;
	QString units;
	QString helptext;
	QList<Option*> options;

//	QString value; // deprecated: now uses FreeCAD data structure which is contained in TPGSettings.tpgFeature

	Definition(const char *name, const char *label, const SettingType  type, const char *defaultvalue, const char *units, const char *helptext);
	Definition(QString name, QString label, SettingType type, QString defaultvalue, QString units, QString helptext);
	Definition();

	~Definition();

	ValidationState validate(QString & input, int & position) const;
	ValidationState validateText(QString & input, int & position) const;
	ValidationState validateObjectNamesForType(QString & input, int & position) const;
	ValidationState validateEnumeration(QString & input,int & position) const;
	ValidationState validateLength(QString & input,int & position) const;
	ValidationState validateFilename(QString & input,int & position) const;
	ValidationState validateDirectory(QString & input,int & position) const;
	ValidationState validateColor(QString & input,int & position) const;
	ValidationState validateInteger(QString & input,int & position) const;
	ValidationState validateDouble(QString & input,int & position) const;

	/**
	 * Perform a deep copy of this class
	 */
    Definition* clone();

    void print() const;

    /**
     * Increases reference count
     * Note: it returns a pointer to this for convenience.
     */
    Definition *grab();

    /**
     * Decreases reference count and deletes self if no other references
     */
    void release();

    /// add an option for the value of this setting
    void addOption(QString id, QString label);
    /// add an option for the value of this setting
    void addOption(const char *id, const char *label);

    /// get the value associated with this setting
    QString getValue() const;

    /// set the value associated with this setting
    bool setValue(QString value);

    /// get the namespaced name <action>::<name>
    QString getFullname() const;

	/// Used by the TPGSetting::EvaluateWithPython() method.
	bool AddToPythonDictionary(PyObject *dictionary, const QString requested_units, const QString prefix) const;
};

/** 
	Class stores hash of settings for managing each independant TPG

	This class mostly just holds a map of Definition objects.
	The Definition objects hold data 'about' the setting but
	the value itself is stored in one of the member variables contained
	within the owning TPGFeature object.
 */
class CamExport TPGSettings
{
public:
    TPGSettings();
    ~TPGSettings();

    void initialise() {};
    void loadSettings() {};

	/**
		called when any one of the settings contained within the TPGFeature::PropTPGSettings 
		member variable changes.  These are called from the TPGFeature::onBeforeChange() and
		TPGFeature::onChanged() methods respectively.
	 */
	void onBeforePropTPGSettingsChange(const App::PropertyMap* prop);
	void onPropTPGSettingsChanged(const App::PropertyMap* prop);

	/**
	 * Perform a deep copy of this class
	 */
    TPGSettings* clone();

    /**
     * Add a setting to this Settings collection
     */
    Definition* addSettingDefinition(QString action, Definition* setting);

    /**
     * Get the currently selected action.
     */
    const QString getAction() const;

    /**
     * Change the currently selected action.
     */
    bool setAction(QString action);

    /**
     * Get the value of a given setting (by name)
     */
    const QString getValue(QString name) const;

    /**
     * Get the value of a given setting (by name)
     */
    const QString getValue(QString action, QString name) const;

    /**
     * Print the settings to stdout
     */
    void print() const;

    std::vector<Definition*> getSettings() const;

    /**
     * Sets the default value for every setting in Settings collection
     */
    void addDefaults();

    /**
     * Get the action names defined
     */
    QStringList getActions() const;

    /**
     * Set the value for the named setting
     */
    bool setValue(QString action, QString name, QString value);

    /**
     * Set the value for the named setting
     */
    bool setValue(QString name, QString value);

    /**
     * Sets the TPGFeature that the value will be saved-to/read-from.
     */
    void setTPGFeature(Cam::TPGFeature *tpgFeature);

    /**
     * Increases reference count
     * Note: it returns a pointer to this for convenience.
     */
    TPGSettings *grab();

    /**
     * Decreases reference count and deletes self if no other references
     */
    void release();

	bool EvaluateLength( const Definition *definition, const char *entered_value, double *pResult ) const;
	bool EvaluateWithPython( const Definition *definition, QString value, QString & evaluated_version ) const;
	bool AddToPythonDictionary(PyObject *dictionary, const QString requested_units, const QString prefix) const;

protected:

    /// the action that is selected (i.e. algorithm and group of settings to use)
    QString action;
    /// the settings in the order that the TPG developer wants them to display
    std::vector<Definition*> settingDefs;
    /// the same settings but in map data structure for random access (key: <action>::<name>)
    std::map<QString, Definition*> settingDefsMap;
    /// a map to store which settings are in which action
    std::map<QString, std::vector<Definition*> > settingDefsActionMap;
    /// reference counter
    int refcnt;
    /// the tpgFeature to which these settings belong
	Cam::TPGFeature *tpgFeature;

    /// make a namespaced name (from <action>::<name>)
    QString makeName(QString action, QString name) const;

private:
	// NOTE: ONLY used to determine which properties changed in a single update.  i.e. this
	// value is quite transient.  It only makes sense when comparing the values written
	// between the onBeforePropTPGSettingsChange() and onPropTPGSettingsChanged() method
	// calls.
	std::map<std::string,std::string>	previous_tpg_properties_version;
};



// Declare some classes used to get/set Definition values depending on the
// type of Definition object used.  Consider these 'helper classes' that
// will allow a Cam::Setting::Definition to be correctly set and interpreted
// when the various types of settings are defined.

class CamExport Color : public Definition
{
public:
	Color(const char *name, const char *label, const char *helptext ):
	  Definition(name, label, SettingType_Color, "", "", helptext)
	{
	}

	bool get(int &red, int &green, int &blue, int &alpha) const;
	void set(const int red, const int green, const int blue, const int alpha);
};

class CamExport Length : public Definition
{
public:
	Length(	const char *name, 
								const char *label, 
								const char *helptext,
								const double default_value,
								const double minimum, 
								const double maximum, 
								const Definition::Units_t units );
	Length(	const char *name, 
								const char *label, 
								const char *helptext,
								const double default_value,
								const Definition::Units_t units );

	bool Evaluate( const char *entered_value, double *pResult ) const;
};

class CamExport Double : public Definition
{
public:
	Double(	const char *name, 
								const char *label, 
								const char *helptext,
								const double default_value,
								const double minimum, 
								const double maximum, 
								const char *units );
	Double(	const char *name, 
								const char *label, 
								const char *helptext,
								const double default_value,
								const char *units );

	bool Evaluate( const char *entered_value, double *pResult ) const;
};

class CamExport ObjectNamesForType : public Definition
{
public:
	ObjectNamesForType(	const char *name, 
								const char *label, 
								const char *helptext,
								const char *delimiters,
								const char *object_type );

	void Add(const char * object_type);
	void SetDelimiters(const char * object_type);

	QStringList GetTypes() const;
	Option *GetDelimitersOption() const;
	QStringList GetNames() const;
};


} // namespace Settings
} //namespace Cam


#endif //CAM_TPGSETTINGS_H
