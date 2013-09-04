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

#include <PreCompiled.h>
#ifndef _PreComp_
#endif

#include "TPGSettings.h"

#include <cstdio>
#include <QList>

#include <Base/Console.h>

namespace Cam {

const char* ts(QString str)
{
	return str.toAscii().constData();
}

const char* ts(QString *str)
{
	if (str != NULL)
		return str->toAscii().constData();
	return "NULL";
}

////////// TPGSetting //////////

TPGSettingDefinition::TPGSettingDefinition(const char *name, const char *label, const SettingType type, const char *defaultvalue, const char *units, const char *helptext)
{
	this->refcnt = 1;
    this->parent = NULL;

	this->name = QString::fromAscii(name);
	this->label = QString::fromAscii(label);
	this->type = type;
	this->defaultvalue = QString::fromAscii(defaultvalue);
	this->units = QString::fromAscii(units);
	this->helptext = QString::fromAscii(helptext);
}
TPGSettingDefinition::TPGSettingDefinition(QString name, QString label, SettingType type, QString defaultvalue, QString units, QString helptext)
{
	this->refcnt = 1;
    this->parent = NULL;

	this->name = name;
	this->label = label;
	this->type = type;
	this->defaultvalue = defaultvalue;
	this->units = units;
	this->helptext = helptext;
}
TPGSettingDefinition::TPGSettingDefinition() {
	this->refcnt = 1;
    this->parent = NULL;
}

TPGSettingDefinition::~TPGSettingDefinition() {
    QList<TPGSettingOption*>::iterator it = this->options.begin();
    for (; it != this->options.end(); ++it)
	{
        delete *it;
	}
    options.clear();
}

bool TPGSettingDefinition::operator== ( const TPGSettingDefinition & rhs ) const
{
	if (name != rhs.name) return(false);
	if (label != rhs.label) return(false);
	if (type != rhs.type) return(false);

	return(true);
}

/**
 * Perform a deep copy of this class
 */
TPGSettingDefinition* TPGSettingDefinition::clone()
{
    TPGSettingDefinition* clone = new TPGSettingDefinition(name, label, type, defaultvalue, units, helptext);
    QList<TPGSettingOption*>::iterator it = this->options.begin();

    for (; it != this->options.end(); ++it)
        clone->addOption((*it)->id, (*it)->label);

	clone->action = this->action;
	clone->parent = this->parent;

    return clone;
}

/**
 * add an option for the value of this setting
 */
void TPGSettingDefinition::addOption(QString id, QString label) {
    this->options.append(new TPGSettingOption(id, label));
}

/**
 * add an option for the value of this setting
 */
void TPGSettingDefinition::addOption(const char *id, const char *label) {
    this->options.append(new TPGSettingOption(id, label));
}


void TPGSettingDefinition::print()
{
	qDebug("  - (%s, %s, %d, %s, %s, %s)\n",
			name.toAscii().constData(),
			label.toAscii().constData(),
			type,
			defaultvalue.toAscii().constData(),
			units.toAscii().constData(),
			helptext.toAscii().constData());
}

/**
 * Increases reference count
 * Note: it returns a pointer to 'this' for convenience.
 */
TPGSettingDefinition *TPGSettingDefinition::grab() {
    refcnt++;
    return this;
}

/**
 * Decreases reference count and deletes self if no other references
 */
void TPGSettingDefinition::release() {
    refcnt--;
    if (refcnt == 0)
        delete this;
}

/**
 * Get the value associated with this setting
 */
QString TPGSettingDefinition::getValue() {
	if (this->parent != NULL)
		return this->parent->getValue(action, name);
	return QString::null;
}

/**
 * Set the value associated with this setting
 */
bool TPGSettingDefinition::setValue(QString value) {
	if (this->parent != NULL) {
		return parent->setValue(action, name, value);
	}
	Base::Console().Warning("Setting doesn't have parent!\n");
	return false;
}

/**
 * Get the namespaced name <action>::<name>
 */
QString TPGSettingDefinition::getFullname() {
	return action + QString::fromAscii("::") + name;
}

// ----- TPGSettings ----------------------------------------------------------

TPGSettings::TPGSettings()
{
	this->refcnt = 1;
	this->tpgFeature = NULL;
}

TPGSettings::~TPGSettings()
{
	// The objects in the settingDefs vector used grab() to increment the reference count when they
	// were added in the addSettingDefinition() method.  Release this reference count now that we
	// don't need them any more.
	for (std::vector<TPGSettingDefinition*>::iterator itSetting = settingDefs.begin(); itSetting != settingDefs.end(); /* increment within loop */ )
	{
		(*itSetting)->release();
		itSetting = settingDefs.erase(itSetting);
	}
}

/**
 * Perform a deep copy of this class
 */
TPGSettings* TPGSettings::clone()
{
	TPGSettings* settings = new TPGSettings();

	std::vector<TPGSettingDefinition*>::iterator it = this->settingDefs.begin();
	while (it != this->settingDefs.end())
	{
		settings->addSettingDefinition((*it)->action, (*it)->clone());
		++it;
	}

	settings->tpgFeature = this->tpgFeature;

	return settings;
}

/**
 * Adds the setting to this setting group
 */
TPGSettingDefinition* TPGSettings::addSettingDefinition(QString action, TPGSettingDefinition* setting) {

	if (setting != NULL) {
		QString qname = makeName(action, setting->name);

		// store reference to setting
		settingDefs.push_back(setting->grab());
		settingDefsMap.insert(std::pair<QString, TPGSettingDefinition*>(qname, setting));

		// take ownership of setting
		setting->action = action;
		setting->parent = this;

		// add setting to the required action
		std::map<QString, std::vector<TPGSettingDefinition*> >::iterator it = settingDefsActionMap.find(action);
		if (it == settingDefsActionMap.end()) {
			std::vector<TPGSettingDefinition*> settings;
			settings.push_back(setting);
			settingDefsActionMap.insert(std::pair<QString, std::vector<TPGSettingDefinition*> >(action, settings));
		} else {
			// Check to see if we already have one.  If so, return a pointer to it rather than adding this new one.
			bool found = false;
			for (std::vector<Cam::TPGSettingDefinition *>::iterator itSettingDef = it->second.begin(); (! found) && (itSettingDef != it->second.end()); itSettingDef++)
			{
				if (*itSettingDef == *setting)
				{
					found = true;
					delete setting;	// Delete this new one.
					setting = &(*itSettingDef);
					break;
				}
			}

			if (! found)
			{
				it->second.push_back(setting);
			}
		}
	}

	// return setting for convenience
	return setting;
}

/**
 * Get the currently selected action.
 */
const QString TPGSettings::getAction() const {
	return action;
}

/**
 * Change the currently selected action.
 */
bool TPGSettings::setAction(QString action) {
	this->action = action;
	if (tpgFeature != NULL)
	{
		tpgFeature->PropTPGSettings.setValue("action", action.toAscii().constData());
		return(true);
	}
	return(false);
}

/**
 * Get the value of a given setting (by name)
 */
const QString TPGSettings::getValue(QString name) {
	if (action.isNull() || action.isEmpty())
		return QString::null;
	return getValue(name, action);
}

/**
 * Get the value of a given setting (by name)
 */
const QString TPGSettings::getValue(QString action, QString name) {

	if (action.isNull() || action.isEmpty()) {
		Base::Console().Message("action not set\n");
		return QString::null;
	}

	// compute full setting name (<action>::<name>)
	QString qname = makeName(action, name);

    // get setting value
	if (tpgFeature != NULL) {
		if (settingDefsMap.find(qname) != settingDefsMap.end()) {
			const std::map<std::string,std::string> vals = tpgFeature->PropTPGSettings.getValues();
			std::map<std::string,std::string>::const_iterator val = vals.find(qname.toStdString());
			if (val != vals.end()) {
				return QString::fromStdString(val->second);
			}
		}
	}
	Base::Console().Message("Can't find setting!\n");
    return QString::null;
}

/**
 * Set the value for the named setting
 */
bool TPGSettings::setValue(QString action, QString name, QString value) {

	if (action.isNull() || action.isEmpty())
		return false;

	// compute full setting name (<action>::<name>)
	QString qname = makeName(action, name);

	if (tpgFeature != NULL) {
		if (settingDefsMap.find(qname) != settingDefsMap.end()) {
			std::string strname = qname.toStdString();
			std::string strvalue = value.toStdString();
			tpgFeature->PropTPGSettings.setValue(strname, strvalue);
			return true;
		}
	}
	Base::Console().Message("Can't find setting!\n");
	return false;
}

/**
 * Set the value for the named setting
 */
bool TPGSettings::setValue(QString name, QString value) {
	return setValue(action, name, value);
}

void TPGSettings::print()
{
	std::vector<TPGSettingDefinition*>::iterator it = this->settingDefs.begin();
	while (it != this->settingDefs.end())
	{
		(*it)->print();
		++it;
	}
}


std::vector<TPGSettingDefinition*> TPGSettings::getSettings() 
{
	return this->settingDefs;
}

/**
 * Sets the default value for any settings that are missing from the TPGFeature
 */
void TPGSettings::addDefaults() {

	if (tpgFeature != NULL) {
		const std::map<std::string,std::string> currentValues = tpgFeature->PropTPGSettings.getValues();

		// add action
		if (currentValues.find("action") == currentValues.end())
			tpgFeature->PropTPGSettings.setValue("action", "default");

		// add all settings for each available action
		std::vector<TPGSettingDefinition*>::iterator it = this->settingDefs.begin();
		while (it != this->settingDefs.end()) {
			QString nsName = makeName((*it)->action, (*it)->name);
			if (currentValues.find(nsName.toStdString()) == currentValues.end())
			{
				tpgFeature->PropTPGSettings.setValue(nsName.toStdString(),(*it)->defaultvalue.toStdString());
			}

			++it;
		}
	}
}

QStringList TPGSettings::getActions() {

	QStringList result;

	std::map<QString, std::vector<TPGSettingDefinition*> >::iterator it = settingDefsActionMap.begin();
	while (it != settingDefsActionMap.end()) {
		Base::Console().Log("%s\n", (*it).first.toAscii().constData());
		result.append((*it).first);
		++it;
	}

	return result;
}

/**
 * Sets the TPGFeature that the value will be saved-to/read-from.
 *
 * This will set the default values for any missing settings
 */
void TPGSettings::setTPGFeature(TPGFeature *tpgFeature) {
	this->tpgFeature = tpgFeature;

	// load the TPG's action
	tpgFeature->PropTPGSettings.getValues();

	addDefaults();
}

/**
 * Increases reference count
 * Note: it returns a pointer to this for convenience.
 */
TPGSettings *TPGSettings::grab() {
    refcnt++;
    return this;
}

/**
 * Decreases reference count and deletes self if no other references
 */
void TPGSettings::release() {
    refcnt--;
    if (refcnt == 0)
        delete this;
}

/**
 * Make a namespaced name (from <action>::<name>)
 */
QString TPGSettings::makeName(QString action, QString name) const {
	QString result = action;
	result.append(QString::fromAscii("::"));
	result.append(name);
	return result;
}


/**
	Keep a copy of the properties map before any changes occur so that we can
	compare it with the map of modified settings.  Only by comparing these two
	can we figure out which one of the settings changed.
 */
void TPGSettings::onBeforePropTPGSettingsChange(const App::PropertyMap* property_map)
{
	if (property_map != NULL)
	{
		this->previous_tpg_properties_version.clear();

		std::copy( property_map->getValues().begin(), property_map->getValues().end(),
			std::inserter( this->previous_tpg_properties_version, this->previous_tpg_properties_version.begin() ) );

		// qDebug("TPGSettings::onBeforePropTPGSettingsChange(%s) called\n", property_map->getName());
	}
}

/**
	Called when one of the TPGFeature::PropTPGSettings values changes.
 */
void TPGSettings::onPropTPGSettingsChanged(const App::PropertyMap* property_map)
{
	// One of the settings has changed.  Figure out which one and let any interested parties know.
	if (tpgFeature != NULL)
	{
		TPG *tpg = tpgFeature->getTPG();
		if (tpg)
		{
			tpg->grab();

			// qDebug("TPGSettings::onPropTPGSettingsChanged() called\n");
			for (std::map<QString, TPGSettingDefinition*>::iterator itSettingsDef = settingDefsMap.begin(); itSettingsDef != settingDefsMap.end(); itSettingsDef++)
			{
				std::string name = itSettingsDef->first.toStdString();
				std::string previous_value = this->previous_tpg_properties_version[name];
				std::string new_value = itSettingsDef->second->getValue().toStdString();

				if (new_value != previous_value)
				{
					tpg->onChanged( itSettingsDef->second, QString::fromStdString(previous_value), QString::fromStdString(new_value));
				}
			}
			tpg->release();
		}
	}
}


} // end namespace Cam