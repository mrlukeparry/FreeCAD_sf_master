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

#include "../TPGFeature.h"
#include "TPG.h"


using namespace Cam;
TYPESYSTEM_SOURCE_ABSTRACT(Cam::TPG, Base::BaseClass)

TPG::TPG()
//    : state(LOADED)
{
    // Load the TPG Settings Class and Initialise
//    settings = new TPGSettings();
//    settings->initialise();
//    settings->loadSettings();

    // Load the TPG Cache and initialise
//    cache = new TPGCache();
//    cache->initialise();

    refcnt = 1;
}

TPG::TPG(const QString &TPGId, const QString &TPGName, const QString &TPGDescription)
{
  this->id = TPGId;
  this->name = TPGName;
  this->description = TPGDescription;
}

TPG::~TPG()
{
}

void TPG::initialise(TPGFeature *feat)
{
//    // We need to intiailise and associate this toolpath with a TPGFeature - it doesn't make sense to be standalone
//    if(!feat)
//        return;
//
//    tpgFeat = feat;

    this->initialiseSettings();
    //Check if there is input that can be used

//    // If everything is okay, set status to intiailised
//    this->state = INITIALISED;
}

//TPG* TPG::makeTPG()
//{
//    return 0;
//}

void TPG::initialiseSettings()
{

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
 * Get the settings for a given action
 */
TPGSettings *TPG::getSettings(QString &action)
{
	std::map<QString, TPGSettings*>::iterator it;
	it = this->settings.find(action);
	if (it != this->settings.end())
		return it->second->clone();
	return NULL;
}

/**
 * Run the TPG to generate the ToolPath code.
 *
 * Note: the return will change once the TP Language has been set in stone
 */
void TPG::run(TPGSettings *settings, QString action="default")
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

