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

#include "../PreCompiled.h"
#ifndef _PreComp_
#endif

#include "TPGSettings.h"

#include <cstdio>

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
}
TPGSetting::TPGSetting(QString &name, QString &label, QString &type, QString &defaultvalue, QString &units, QString &helptext)
{
	this->name = name;
	this->label = label;
	this->type = type;
	this->defaultvalue = defaultvalue;
	this->units = units;
	this->helptext = helptext;
}

/**
 * Perform a deep copy of this class
 */
TPGSetting* TPGSetting::clone()
{
	return new TPGSetting(name, label, type, defaultvalue, units, helptext);
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

////////// TPGSettings /////////

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
void TPGSettings::addSetting(TPGSetting* setting)
{
	this->settings.push_back(setting);
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

}
