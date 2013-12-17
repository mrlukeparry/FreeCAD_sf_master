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

#ifndef CAM_TOOLFEATURE_H
#define CAM_TOOLFEATURE_H

//#define CamExport
#include <PreCompiled.h>

namespace Cam {
class CamExport ToolFeature;
}

#include <boost/signals.hpp>

#include <App/DocumentObject.h>
#include <App/PropertyLinks.h>
#include <App/PropertyStandard.h>
#include <Base/BoundBox.h>
#include <Mod/Part/App/PartFeature.h>

#include "../Features/CamSettingsableFeature.h"
#include "../TPG/TPGSettings.h"

typedef boost::signals::connection Connection;

/**
  * ToolFeature is the document object to store details of a single CAM Tool
  */
namespace Cam
{
class CamExport ToolFeature : public Cam::Settings::Feature
{
    PROPERTY_HEADER(Cam::ToolFeature);

public:
    ToolFeature();
    ~ToolFeature();

    App::PropertyString        ToolId;

    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute(void);

    const char* getViewProviderName(void) const {
        return "CamGui::ViewProviderToolFeature";
    }

	// From Cam::Settings::Feature
	virtual Cam::Settings::TPGSettings *getTPGSettings();
	virtual void onSettingChanged(const std::string key, const std::string previous_value, const std::string new_value);

	void initialise();

    void onDelete(const App::DocumentObject &docObj);

//    virtual void Save(Base::Writer &/*writer*/) const;
//    virtual void Restore(Base::XMLReader &/*reader*/);

protected:
    
    ///Connections
    Connection delObjConnection;

    /// get called by the container when a property has changed
    //     virtual void onChanged(const App::Property* /*prop*/);
    virtual void onSettingDocument();

//    virtual void onDocumentRestored();

public:
	typedef enum {
		eHighSpeedSteel = 0,
		eCarbide,
		eUndefinedMaterialType
	} eMaterial_t;

	friend QString & operator << ( QString & qs, const eMaterial_t & material_type )
	{
		std::ostringstream ss;
		switch (material_type)
		{
		case eHighSpeedSteel:		ss << "HSS";
			break;

		case eCarbide:	ss << "Carbide";
			break;

		case eUndefinedMaterialType:	ss << "Undefined";
			break;
		} // End switch()

		qs.append( QString::fromStdString(ss.str()));
		return(qs);
	}

	typedef enum {
		eDrill = 0,
		eCentreDrill,
		eEndmill,
		eSlotCutter,
		eBallEndMill,
		eChamfer,
		eTurningTool,
		eTouchProbe,
		eToolLengthSwitch,
		eExtrusion,
		eTapTool,
		eEngravingTool,
		eBoringHead,
		eDragKnife,
		eUndefinedToolType
	} eToolType;

	friend QString & operator<< ( QString & qs, const eToolType & tool_type )
	{
		std::ostringstream ss;
		switch (tool_type)
		{
		case eDrill:		ss << "Drill";
			break;

		case eCentreDrill:	ss << "Centre Drill";
			break;

		case eEndmill:	ss << "Endmill";
			break;

		case eSlotCutter:	ss << "Slot Cutter";
			break;

		case eBallEndMill:	ss << "Ball Endmill";
			break;

		case eChamfer:	ss << "Chamfer";
			break;

		case eTurningTool:	ss << "Turning Tool";
			break;

		case eTouchProbe:	ss << "Touch Probe";
			break;

		case eToolLengthSwitch:	ss << "Tool Length Switch";
			break;

		case eExtrusion:	ss << "Extruder";
			break;

		case eTapTool:	ss << "Tap";
			break;

		case eEngravingTool:	ss << "Engraving Tool";
			break;

		case eBoringHead:	ss << "Boring Head";
			break;

		case eDragKnife:	ss << "Drag Knife";
			break;

		case eUndefinedToolType: ss << "Undefined tool type";
			break;
		} // End switch()

		qs.append( QString::fromStdString(ss.str()) );
		return(qs);
	}

	
public:
	Settings::Length	*diameter;
	Settings::Length	*tool_length_offset;
	Settings::Enumeration *material;
	Settings::Enumeration *type;

private:
	Settings::TPGSettings *settings;
};

} //namespace Cam


#endif //CAM_TOOLFEATURE_H
