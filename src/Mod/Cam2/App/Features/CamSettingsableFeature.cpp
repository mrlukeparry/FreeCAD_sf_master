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

#include <boost/bind.hpp>

#include <App/Application.h>
#include <App/Document.h>
#include <App/PropertyContainer.h>

#include <Base/Console.h>
#include <Base/Reader.h>
#include <Base/Writer.h>

#include "CamSettingsableFeature.h"

using namespace Cam;

PROPERTY_SOURCE(Cam::Settings::Feature, App::DocumentObject)

Settings::Feature::Feature()
{
	//ADD_PROPERTY_TYPE(_prop_, _defaultval_, _group_,_type_,_Docu_)
//    ADD_PROPERTY_TYPE(ToolId,        (""),  "Tool Feature", (App::PropertyType)(App::Prop_ReadOnly) , "Tool ID");
	ADD_PROPERTY_TYPE(Values, (),    "Settingsable Feature", (App::PropertyType)(App::Prop_None) , "Settings storage");
}

Settings::Feature::~Feature()
{
	delObjConnection.disconnect();
}

App::DocumentObjectExecReturn *Settings::Feature::execute(void)
{
    return App::DocumentObject::StdReturn;
}

void Settings::Feature::setValue(const std::string & key, const std::string & value )
{
	Values.setValue(key, value);
}

const std::map<std::string,std::string> &Settings::Feature::getValues(void) const
{
	return Values.getValues();
}


//void Settings::Feature::onSettingDocument()
//{
//    //Create a signal to observe slot if this item is deleted
//    delObjConnection = getDocument()->signalDeletedObject.connect(boost::bind(&Cam::CamSettingsableFeature::onDelete, this, _1));
//}

bool Settings::Feature::IsCamSettingsProperty(const App::Property* prop) const
{
	return(prop == &Values);
}

/**
	Called by the App::Property framework just before a property is changed.
 */
/* virtual */ void Settings::Feature::onBeforeChange(const App::Property* prop)
{
}


/* virtual */ void Settings::Feature::onChanged(const App::Property* prop)
{
}


void Settings::Feature::initialise()
{
	return;
}

//void Settings::Feature::onDelete(const App::DocumentObject &docObj) {
//
//    // If deleted object me, proceed to delete my children
//    const char *myName = getNameInDocument();
//    if(myName != 0 && std::strcmp(docObj.getNameInDocument(), myName) == 0) {
//        App::Document *pcDoc = getDocument();
//
//        // remove the toolpath object if needed
//        if (this->ToolPath.getValue() != NULL) {
//            pcDoc->remObject(this->ToolPath.getValue()->getNameInDocument());
//        }
//    }
//}

//void Settings::Feature::Save(Base::Writer &writer) const
//{
//    //save the father classes
//    App::DocumentObject::Save(writer);
//}

//void Settings::Feature::Restore(Base::XMLReader &reader)
//{
//    //read the father classes
//    App::DocumentObject::Restore(reader);
//}


//void Settings::Feature::onDocumentRestored()
//{
//}
