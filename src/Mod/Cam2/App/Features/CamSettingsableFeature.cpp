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
	ADD_PROPERTY_TYPE(Values, (),    "Cam::Settings::Feature", (App::PropertyType)(App::Prop_None) , "Settings storage");
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
	if (prop == NULL) return(false);
	return(prop == &Values);
}

/**
	Called by the App::Property framework just before a property is changed.

	Keep a copy of the properties map before any changes occur so that we can
	compare it with the map of modified settings.  Only by comparing these two
	can we figure out which one of the settings changed.
 */
void Settings::Feature::onBeforeChange(const App::Property* prop)
{
	if (IsCamSettingsProperty(prop))
	{
		const App::PropertyMap *property_map = dynamic_cast<const App::PropertyMap *>(prop);

		if (property_map != NULL)
		{
			this->previous_values.clear();
			std::copy( property_map->getValues().begin(), property_map->getValues().end(),
				std::inserter( this->previous_values, this->previous_values.begin() ) );

			qDebug("Settings::Feature::onBeforeChange(%s) called\n", prop->getName());
		}
	}
}

/**
	Figure out which of our property types changed and signal the underlying
	TPGSettings object accordingly.

	This method is called by the App::Property framework automatically just
	after a property has changed.

	It's possible that we store some settings in a member variable OTHER than
	the Cam::Settings::Feature::Values member.  If that's the case then this method is the
	place where the association is made.  i.e. we need to figure out which
	member variable holds the modified setting and signal the underlying
	TPGSettings object accordingly.
 */
void Settings::Feature::onChanged(const App::Property* prop)
{
	if (IsCamSettingsProperty(prop))
	{
		// It is one of the properties contained within the Cam::Settings::Feature::Values map...
		const App::PropertyMap *property_map = dynamic_cast<const App::PropertyMap *>(prop);
		if (property_map != NULL)
		{
			// Figure out which value changed and signal the owning object.
			for (std::map<std::string, std::string>::const_iterator itValue = property_map->getValues().begin(); itValue != property_map->getValues().end(); itValue++)
			{
				if ((previous_values.find(itValue->first) != previous_values.end()) &&
					(previous_values[itValue->first] != itValue->second))
				{
					// This value has changed.  Signal the owning object.
					onSettingChanged( itValue->first, previous_values[itValue->first], itValue->second );
				}
			}
		}
	}
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
