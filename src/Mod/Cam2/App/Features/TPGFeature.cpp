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

#include <App/PropertyContainer.h>

#include <Base/Console.h>
#include <Base/Reader.h>
#include <Base/Writer.h>

#include "TPGFeature.h"
#include "../TPG/TPGFactory.h"

#include <App/Document.h>
#include <App/Application.h>

using namespace Cam;

PROPERTY_SOURCE(Cam::TPGFeature, App::DocumentObject)

TPGFeature::TPGFeature() {

	//ADD_PROPERTY_TYPE(_prop_, _defaultval_, _group_,_type_,_Docu_)
    ADD_PROPERTY_TYPE(PluginId,        (""),   "TPG Feature", (App::PropertyType)(App::Prop_ReadOnly) , "Plugin ID");
    ADD_PROPERTY_TYPE(PropTPGSettings,(), "TPG Feature", (App::PropertyType)(App::Prop_None) , "TPG's Settings storage");

    tpg = NULL;
    tpgSettings = NULL; //new TPGSettings();
}

//// TODO not sure if this is actually needed anymore.
//TPGFeature::TPGFeature(TPGDescriptor *tpgDescriptor)
//{
////    loadTPG(tpgDescriptor);
//}

//bool TPGFeature::loadTPG(TPGDescriptor *tpgDescriptor)
//{
//    if(tpgDescriptor == NULL)
//        throw Base::Exception("TPG Plugin Description is null");
//
//    // First check if a plugin already exists and quit if already running
//    if(this->hasRunningTPG())
//        return false;
//
//    // Make the plugin from the descriptor
//    TPG *myTpg = NULL;
//    myTpg = tpgDescriptor->make();
//
//    if(myTpg == NULL)
//        return false;
//
//    // Set the TPGFeatures internal TPG member
//    this->tpg = myTpg;
//
//    // Set the PluginID Property
//    PluginId.setValue(myTpg->getId().toStdString());
//    return true;
//}
//
//void TPGFeature::run()
//{
//      // Load the TPG, this
//     if(hasTPG()/*&& tpg->isReady()*/) {
//       //Initialise the TPG for running
////        this->tpg->setInputBBox(inputBBox); // Set the bounding box could be null
//      std::stringstream ss;
//      ss << "Running : " << this->tpg->getName().toStdString() << "\n";
//      Base::Console().Log(ss.str().c_str());
//       //this->tpg->run();
//   } else
//       Base::Console().Log("The TPG is not ready");
//}
//
//void TPGFeature::stop()
//{
////    if(hasRunningTPG())
////        this->tpg->stop();
////    else
////        Base::Console().Log("The TPG is not running");
//}
//
//TPG::State TPGFeature::getTPGStatus() {
////    return (hasTPG()) ?  this->tpg->getState() : TPG::UNDEFINED;
//	return TPG::UNDEFINED;
//}
//
//bool TPGFeature::hasRunningTPG()
//{
//    TPG::State tpgStatus = getTPGStatus();
//    return (tpgStatus == TPG::RUNNING || tpgStatus == TPG::STARTED);
//}

TPGFeature::~TPGFeature()
{
//    // If the TPG is running we must stop this (especially if a (network) process or thread
//    if(this->hasRunningTPG())
//        stop();
//    //TODO should we wait till the tpg has finished
//    tpg->release(); // Will internally call destructor and safely stop this

	if (tpg != NULL)
	{
		tpg->release();
		tpg = NULL;
	}

	if (tpgSettings != NULL)
	{
		tpgSettings->release();
		tpgSettings = NULL;
	}
}

App::DocumentObjectExecReturn *TPGFeature::execute(void)
{
    Base::Console().Log("Running Feature \n");
//    this->run();
    return App::DocumentObject::StdReturn;
}

void TPGFeature::onBeforeChange(const App::Property* prop)
{
	if (prop == &PropTPGSettings)
	{
		const App::PropertyMap *property_map = dynamic_cast<const App::PropertyMap *>(prop);
		if (property_map)
		{
			// Let the tpgSettings object know that something is about to change.
			if (tpgSettings != NULL)
			{
				tpgSettings->onBeforePropTPGSettingsChange(property_map);
			}
		}
	}
}


void TPGFeature::onChanged(const App::Property* prop)
{
	if (prop == &PropTPGSettings)
	{
		const App::PropertyMap *property_map = dynamic_cast<const App::PropertyMap *>(prop);
		if (property_map)
		{
			// Let the tpgSettings object know that something changed.
			if (tpgSettings != NULL)
			{
				tpgSettings->onPropTPGSettingsChanged(property_map);
			}
		}
	}
}



/**
 * Get a reference to the TPG that implements this TPG Feature.
 * Will load the TPG instance if not set already.
 */
TPG* TPGFeature::getTPG() {
	if (tpg == NULL)
		tpg = TPGFactory().getPlugin(QString::fromStdString(PluginId.getStrValue()));
	return tpg;
}


/**
 * Get the current TPG settings object
 */
TPGSettings* TPGFeature::getTPGSettings() {

	if (tpgSettings == NULL) {
		getTPG(); // make sure TPG has been loaded already.

		// get a description of settings that TPG expects
		tpgSettings = tpg->getSettingDefinitions();
		if (tpgSettings == NULL) {
			Base::Console().Warning("Unable to get settings\n");
			return NULL;
		}
		else
			Base::Console().Log("Got settings\n");

		// tell the settings object about myself so it can save values here.
		tpgSettings->setTPGFeature(this);
	}

	return tpgSettings;
}

void TPGFeature::Save(Base::Writer &writer) const
{
	//NOTE: this isn't need anymore as the setting values are placed directly into the PropTPGSettings object
//	// save the settings into the property before saving
//	if (tpgSettings != NULL) {
//		std::map<std::string,std::string> newVals;
//		std::vector<TPGSettingDefinition*> settings = tpgSettings->getSettings();
//		for (std::vector<TPGSettingDefinition*>::iterator it = settings.begin(); it != settings.end(); ++it) {
//			std::string name = (*it)->name.toStdString();
//			std::string value = (*it)->value.toStdString();
//			newVals[name] = value;
//		}
////		PropTPGSettings.setValues(newVals);
//	}

    //save the father classes
    App::DocumentObject::Save(writer);
}

//void TPGFeature::Restore(Base::XMLReader &reader)
//{
//    //read the father classes
//    App::DocumentObject::Restore(reader);
//}

void TPGFeature::onDocumentRestored()
{
    // Attempt to Load the plugin - guarantee no TPG is loaded
    // TODO not sure if we should catch this higher up but this is the only place
//    try {
//      std::string id = PluginId.getValue();
//      if(id.length() > 0) {
//        // Load the TPG Plugin
//        TPG *tpgPlugin = TPGFactory().getPlugin(QString::fromStdString(id));
//        if(tpgPlugin == NULL)
//          throw Base::Exception("Plugin couldn't be loaded");
//
//        this->tpg = tpgPlugin;
//      }
//    } catch (...) {
//
//    }
}