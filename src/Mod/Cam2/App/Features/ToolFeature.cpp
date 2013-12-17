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

#include "ToolFeature.h"

using namespace Cam;

PROPERTY_SOURCE(Cam::ToolFeature, Cam::Settings::Feature)

ToolFeature::ToolFeature()
{
	//ADD_PROPERTY_TYPE(_prop_, _defaultval_, _group_,_type_,_Docu_)
    ADD_PROPERTY_TYPE(ToolId, (""), "Tool Feature", (App::PropertyType)(App::Prop_ReadOnly),
            "Unique Tool Identifier");

	this->settings = new Settings::TPGSettings;
	this->settings->setFeature(this);

	diameter = NULL;
	tool_length_offset = NULL;
	material = NULL;
	type = NULL;
}

ToolFeature::~ToolFeature()
{
	delObjConnection.disconnect();

	if (diameter) diameter->release();
	if (tool_length_offset) tool_length_offset->release();
	if (material) material->release();
	if (type) type->release();
}

App::DocumentObjectExecReturn *ToolFeature::execute(void)
{
    return App::DocumentObject::StdReturn;
}

void ToolFeature::onSettingDocument()
{
    //Create a signal to observe slot if this item is deleted
    delObjConnection = getDocument()->signalDeletedObject.connect(boost::bind(&Cam::ToolFeature::onDelete, this, _1));
}

// From Cam::Settings::Feature
/* virtual */ Cam::Settings::TPGSettings *ToolFeature::getTPGSettings()
{
	this->initialise();

	if (this->settings != NULL)
	{
		return(this->settings->grab());
	}
	else
	{
		return(NULL);
	}
}


// From Cam::Settings::Feature
/* virtual */ void ToolFeature::onSettingChanged(const std::string key, const std::string previous_value, const std::string new_value)
{
	if (this->settings != NULL)
	{
		Cam::Settings::Definition *definition = this->settings->getDefinition( QString::fromStdString(key) );
		if (definition != NULL)
		{

		}
	}
}


void ToolFeature::initialise()
{
	QString qaction = QString::fromAscii("default");

	if ((settings != NULL) && (this->diameter == NULL))
	{
		this->diameter = new Settings::Length(	"Diameter", 
											 "Cutting diameter",
											 "Cutting diameter",
											 5.0,
											 Settings::Definition::Metric );
		this->diameter->Minimum(0.0);	// must be positive.  No maximum.
		settings->addSettingDefinition(qaction, this->diameter);

		this->tool_length_offset = new Settings::Length( "Tool Length Offset", 
											 "Tool Length Offset",
											 "Full length of the tool.  Includes both cutting and non-cutting areas of the tool.  Used for rendering of the tool solid.",
											 5.0,
											 Settings::Definition::Metric );
		this->tool_length_offset->Minimum(0.0);	// must be positive.  No maximum.
		settings->addSettingDefinition(qaction, this->tool_length_offset);

		this->material = new Settings::Enumeration(	 "Material", 
													 "Material",
													 int(eHighSpeedSteel),
													 "HSS or Carbide",
													 "Material with which the tool is made.  eg: High Speed Steel or Carbide.  Used for spindle speed estimation.");

		// Enumerated types MUST have one option for each different value.  For each option, the Id must be the integer form and the Label must
		// be the string (verbose) form.  Only the verbose forms are used on the user interface but the values used in the TPGSettingDefinition.value will
		// always be the integer form.
		// The integer forms need not start from zero or be sequential.  The values will appear in the combo-box in the order that
		// they're defined in the options list.  Their position in the list will be used by the combo-box.

		for (eMaterial_t mat = eHighSpeedSteel; mat < eUndefinedMaterialType; mat = eMaterial_t(int(mat)+1))
		{
			QString label;
			label << mat;		// use the operator<< override to convert from the enum to the string form.

			this->material->Add(int(mat), label);
		}

		settings->addSettingDefinition(qaction, this->material);


		this->type = new Settings::Enumeration(	 "Type", 
												 "Type",
												 int(eDrill),
												 "Type of tool",
												 "Type of tool");

		// Enumerated types MUST have one option for each different value.  For each option, the Id must be the integer form and the Label must
		// be the string (verbose) form.  Only the verbose forms are used on the user interface but the values used in the TPGSettingDefinition.value will
		// always be the integer form.
		// The integer forms need not start from zero or be sequential.  The values will appear in the combo-box in the order that
		// they're defined in the options list.  Their position in the list will be used by the combo-box.

		for (eToolType tool_type = eDrill; tool_type < eUndefinedToolType; tool_type = eToolType(int(tool_type)+1))
		{
			QString label;
			label << tool_type;		// use the operator<< override to convert from the enum to the string form.

			this->type->Add(int(tool_type), label);
		}

		settings->addSettingDefinition(qaction, this->type);
	}
}

void ToolFeature::onDelete(const App::DocumentObject &docObj) {

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
}

//void ToolFeature::Save(Base::Writer &writer) const
//{
//    //save the father classes
//    App::DocumentObject::Save(writer);
//}

//void TPGFeature::Restore(Base::XMLReader &reader)
//{
//    //read the father classes
//    App::DocumentObject::Restore(reader);
//}


//void ToolFeature::onDocumentRestored()
//{
//}
