/***************************************************************************
 *   Copyright (c) 2012 Luke Parry    (l.parry@warwick.ac.uk)              *
 *   Copyright (c) 2013 Andrew Robinson <andrewjrobinson@gmail.com>        *
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

#include "../Features/TPGFeature.h"
#include "TPG.h"


using namespace Cam;
TYPESYSTEM_SOURCE_ABSTRACT(Cam::TPG, Base::BaseClass)

TPG::TPG(TPGFeature *tpgFeature)
//    : state(LOADED)
{
    // Load the TPG Settings Class and Initialise
//    settings = new TPGSettings();
//    settings->initialise();
//    settings->loadSettings();

    // Load the TPG Cache and initialise
//    cache = new TPGCache();
//    cache->initialise();

    this->refcnt = 1;
    this->settings = NULL;
	this->tpgFeature = tpgFeature;
}

TPG::TPG(const QString &TPGId, const QString &TPGName, const QString &TPGDescription, TPGFeature *tpgFeature)
{
  this->refcnt = 1;
  this->id = TPGId;
  this->name = TPGName;
  this->description = TPGDescription;
  this->settings = NULL;
  this->tpgFeature = tpgFeature;
}

TPG::~TPG()
{
	if (this->settings) this->settings->release();
	// no need to release the tpgFeature as it's not reference counted.
}

//TPG* TPG::makeTPG()
//{
//    return 0;
//}

/* virtual */ void TPG::initialise(TPGFeature *tpgFeature)
{
	this->tpgFeature = tpgFeature;

	if (this->settings == NULL)
	{
		QString action = QString::fromAscii("default");
		this->settings = new TPGSettings();
		settings->addSettingDefinition(action, 
										new TPGSettingDefinition(settingName_Geometry().toAscii().constData(), 
										"Geometry", TPGSettingDefinition::SettingType_Text, "Box01", "", "The input geometry that should be cut"));

		settings->addSettingDefinition(action, 
										new TPGSettingDefinition(settingName_Tool().toAscii().constData(), 
										"Tool", TPGSettingDefinition::SettingType_Text, "Tool01", "", "The tool to use for cutting"));
	}

	//    // If everything is okay, set status to intiailised
	//    this->state = INITIALISED;
}

QString TPG::settingName_Geometry() const
{
	return(QString::fromAscii("geometry"));
}

QString TPG::settingName_Tool() const
{
	return(QString::fromAscii("tool"));
}

//void TPG::stop()
//{
//
//}
//
//void TPG::getSTLModel() {}



// TPG API methods.  Used by the CAM workbench to run the TPG

/**
 * Returns a list of action names supported by this TPG.
 *
 * Note: if this TPG only does one thing then just return a list containing 'default'
 */
std::vector<QString> &TPG::getActions()
{
	return this->actions;
}

/**
 * Get the settings for this TPG.  Note: value returned is a deep copy of object.
 */
TPGSettings *TPG::getSettingDefinitions()
{
	if (settings != NULL)
		return settings->clone();
	return NULL;
}

/**
 * Run the TPG to generate the ToolPath code.
 *
 * Note: the return will change once the TP Language has been set in stone
 */
void TPG::run(TPGSettings *settings, ToolPath *toolpath, QString action = QString::fromAscii("default"))
{
	return;
}

/**
 * Increases reference count
 */
void TPG::grab() {
    refcnt++;
}

/**
 * Decreases reference count and deletes self if no other references
 */
void TPG::release() {
    refcnt--;
    if (refcnt <= 0)
        delete this;
}

/* static */ QString TPG::stateToStr(State state) {
	if (state == RUNNING)
		return QString::fromAscii("Running");
	else if (state == STARTED)
		return QString::fromAscii("Started");
	else if (state == ERROR)
		return QString::fromAscii("Error");
	else if (state == FINISHED)
		return QString::fromAscii("Finished");
	else if (state == INITIALISED)
		return QString::fromAscii("Initialised");
	else if (state == UNDEFINED)
		return QString::fromAscii("Undefined");
	else if (state == LOADED)
		return QString::fromAscii("Loaded");
	return QString::fromAscii("Undefined");
}


/* virtual */ void TPG::onChanged( TPGSettingDefinition *tpgSettingDefinition, QString previous_value, QString new_value)
{
	qDebug("TPG::onChanged(%s changed from %s to %s)\n", 
				tpgSettingDefinition->getFullname().toAscii().constData(),
				previous_value.toAscii().constData(), 
				new_value.toAscii().constData());
}