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

#include "../PreCompiled.h"
#ifndef _PreComp_
#endif

#include <Base/Console.h>

#include "TPGFeature.h"
#include "../TPG/TPGFactory.h"

using namespace Cam;

PROPERTY_SOURCE(Cam::TPGFeature, App::DocumentObject)

TPGFeature::TPGFeature()// : tpg(NULL)
{
//    ADD_PROPERTY_TYPE(ExternalGeometry,(0, 0), "TPG Feature", (App::PropertyType)(App::Prop_None) , "External geometry");
    ADD_PROPERTY_TYPE(PluginId,        (""),   "TPG Feature", (App::PropertyType)(App::Prop_ReadOnly) , "Plugin ID");
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
}

App::DocumentObjectExecReturn *TPGFeature::execute(void)
{
    Base::Console().Log("Running Feature \n");
//    this->run();
    return App::DocumentObject::StdReturn;
}

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
