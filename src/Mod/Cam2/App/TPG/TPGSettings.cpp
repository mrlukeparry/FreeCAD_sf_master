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

#include "../PreCompiled.h"
#ifndef _PreComp_
#endif

#include "TPGSettings.h"

#include <cstdio>
#include <qlist.h>

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

TPGSetting::TPGSetting(const char *name, const char *label, const char *type, const char *defaultvalue, const char *units, const char *helptext)
{
	this->name = QString::fromAscii(name);
	this->label = QString::fromAscii(label);
	this->type = QString::fromAscii(type);
	this->defaultvalue = QString::fromAscii(defaultvalue);
	this->units = QString::fromAscii(units);
	this->helptext = QString::fromAscii(helptext);
	this->value = QString::fromAscii("test");
}
TPGSetting::TPGSetting(QString &name, QString &label, QString &type, QString &defaultvalue, QString &units, QString &helptext)
{
	this->name = name;
	this->label = label;
	this->type = type;
	this->defaultvalue = defaultvalue;
	this->units = units;
	this->helptext = helptext;
    this->value = QString::fromAscii("test");
}
TPGSetting::TPGSetting() {

}

TPGSetting::~TPGSetting() {
    QList<TPGSettingOption*>::iterator it = this->options.begin();
    for (; it != this->options.end(); ++it)
        delete *it;
    options.clear();
}

/**
 * Perform a deep copy of this class
 */
TPGSetting* TPGSetting::clone()
{
    TPGSetting* clone = new TPGSetting(name, label, type, defaultvalue, units, helptext);
    clone->value = value;
    QList<TPGSettingOption*>::iterator it = this->options.begin();

    for (; it != this->options.end(); ++it)
        clone->addOption((*it)->id, (*it)->value);

    return clone;
}


void TPGSetting::addOption(QString id, QString value) {
    this->options.append(new TPGSettingOption(id, value));
}

void TPGSetting::addOption(const char *id, const char *value) {
    this->options.append(new TPGSettingOption(id, value));
}


void TPGSetting::print()
{
	printf("  - (%s, %s, %s, %s, %s, %s)\n",
			name.toAscii().constData(),
			label.toAscii().constData(),
			type.toAscii().constData(),
			defaultvalue.toAscii().constData(),
			units.toAscii().constData(),
			helptext.toAscii().constData());
}
void TPGSetting::setDefault()
{
    value = defaultvalue;
//    printf("Setting: '%s' to default value (%s) '%s'\n", name.toAscii().constData(), defaultvalue.toAscii().constData(), value.toAscii().constData());
}
// ----- TPGSettings ----------------------------------------------------------

TPGSettings::TPGSettings()
{
}

TPGSettings::~TPGSettings()
{
}

/**
 * Perform a deep copy of this class
 */
TPGSettings* TPGSettings::clone()
{
	TPGSettings* settings = new TPGSettings();

	std::vector<TPGSetting*>::iterator it = this->settings.begin();
	while (it != this->settings.end())
	{
		settings->addSetting((*it)->clone());
		++it;
	}

	return settings;
}

/**
 * Adds the setting to this setting group
 */
TPGSetting* TPGSettings::addSetting(TPGSetting* setting)
{
	this->settings.push_back(setting->grab());
	return setting;
}

/**
 * Get the value of a given setting (by name)
 */
QString *TPGSettings::getSetting(const char *name) {

    QString qname = QString::fromAscii(name);

    std::vector<TPGSetting*>::iterator it = this->settings.begin();
    while (it != this->settings.end()) {
        if ((*it)->name == qname)
            return &((*it)->value);
        ++it;
    }
    return NULL;
}

void TPGSettings::print()
{
	std::vector<TPGSetting*>::iterator it = this->settings.begin();
	while (it != this->settings.end())
	{
		(*it)->print();
		++it;
	}
}



/**
 * Sets the default value for every setting in Settings collection
 */
void TPGSettings::setDefaults()
{
    std::vector<TPGSetting*>::iterator it = this->settings.begin();
    while (it != this->settings.end())
    {
        (*it)->setDefault();
        ++it;
    }
}

} // end namespace Cam
