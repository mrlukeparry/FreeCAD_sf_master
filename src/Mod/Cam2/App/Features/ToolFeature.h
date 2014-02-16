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
	// Define some structures and enumerations to help with subsequent settings definitions.
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

	typedef struct
    {
        QString description;
        double  diameter;
        double  pitch;
    } tap_sizes_t;

	typedef struct
	{
		QString size;
		double body_diameter;
		double drill_diameter;
		double drill_length;
		double overall_length;
	} centre_drill_t;

	typedef enum {
		eHighSpeedSteel = 0,
		eCarbide,
		eUndefinedMaterialType
	} eMaterial_t;

	typedef enum {
		eABS = 0,
		ePLA,
		eHDPE,
		eUndefinedExtrusionMaterialType
	} eExtrusionMaterial_t;

	typedef enum {
		eRightHandThread = 0,
		eLeftHandThread
	} eTappingDirection_t;

	typedef enum {
		eUnused = 0,
		eTurningBackFacing,
		eTurningFacing,
		eBoringFacing,
		eBoringBackFacing,
		eTurning,
		eFacing,
		eBoring,
		eCentre
	} eOrientation_t;
	
private:
	Settings::Text *setup_instructions;	// Note to add to GCode file to make sure the operator sets up the tool correctly at the start.
	Settings::Text *title;					// This reflects the object's name.  It can be automatically or manually generated.

	Settings::Length	*diameter;
	Settings::Length	*tool_length_offset;
	Settings::Enumeration *material;
	Settings::Enumeration *tool_type;

	// The following  properties relate to the extrusions created by a reprap style 3D printer.
	// using temperature, speed, and the height of the nozzle, and the nozzle size it's possible to create
	// many different sizes and shapes of extrusion.

	Settings::Enumeration *extrusion_material;

	/**
		The next three parameters describe the cutting surfaces of the bit.

		The two radii go from the centre of the bit -> flat radius -> corner radius.
		The vertical_cutting_edge_angle is the angle between the centre line of the
		milling bit and the angle of the outside cutting edges.  For an end-mill, this
		would be zero.  i.e. the cutting edges are parallel to the centre line
		of the milling bit.  For a chamfering bit, it may be something like 45 degrees.
		i.e. 45 degrees from the centre line which has both cutting edges at 2 * 45 = 90
		degrees to each other

		For a ball-nose milling bit we would have;
			- corner_radius = m_diameter / 2
			- flat_radius = 0;	// No middle bit at the bottom of the cutter that remains flat
						// before the corner radius starts.
			- cutting_edge_angle = 0

		For an end-mill we would have;
			- corner_radius = 0;
			- flat_radius = m_diameter / 2
			- cutting_edge_angle = 0

		For a chamfering bit we would have;
			- corner_radius = 0;
			- flat_radius = 0;	// sharp pointed end.  This may be larger if we can't use the centre point.
			- cutting_edge_angle = 45	// degrees from centre line of tool
	 */
	Settings::Length	*corner_radius;
	Settings::Length	*flat_radius;
	Settings::Double	*cutting_edge_angle;
	Settings::Length	*cutting_edge_height;	// How far, from the bottom of the cutter, do the flutes extend?

	Settings::Length	*max_advance_per_revolution;	// This is the maximum distance a tool should advance during a single
						// revolution.  This value is often defined by the manufacturer in
						// terms of an advance no a per-tooth basis.  This value, however,
						// must be expressed on a per-revolution basis.  i.e. we don't want
						// to maintain the number of cutting teeth so a per-revolution
						// value is easier to use.

	Settings::Enumeration *automatically_generate_title;	// Set to true by default but reset to false when the user edits the title.

	// The following coordinates relate ONLY to touch probe tools.  They describe
	// the error the probe tool has in locating an X,Y point.  These values are
	// added to a probed point's location to find the actual point.  The values
	// should come from calibrating the touch probe.  i.e. set machine position
	// to (0,0,0), drill a hole and then probe for the centre of the hole.  The
	// coordinates found by the centre finding operation should be entered into
	// these values verbatim.  These will represent how far off concentric the
	// touch probe's tip is with respect to the quil.  Of course, these only
	// make sense if the probe's body is aligned consistently each time.  I will
	// ASSUME this is correct.

	Settings::Length	*probe_offset_x;
	Settings::Length	*probe_offset_y;

	// The gradient is the steepest angle at which this tool can plunge into the material.  Many
	// tools behave better if they are slowly ramped down into the material.  This gradient
	// specifies the steepest angle of decsent.  This is expected to be a negative number indicating
	// the 'rise / run' ratio.  Since the 'rise' will be downward, it will be negative.
	// By this measurement, a drill bit's straight plunge would have an infinite gradient (all rise, no run).
	// To cater for this, a value of zero will indicate a straight plunge.

	Settings::Length	*gradient;

	// properties for tapping tools
	Settings::Enumeration *direction;    // 0.. right hand tapping, 1..left hand tapping
	Settings::Length *pitch;     // in units/rev
	Settings::Enumeration *standard_tap_sizes;

	// properties for centre-drills
	Settings::Enumeration *centre_drill_size;

	// The following are all for lathe tools.  They become relevant when the m_type = eTurningTool
	Settings::Length *x_offset;
	Settings::Double *front_angle;
	Settings::Double *tool_angle;
	Settings::Double *back_angle;
	Settings::Enumeration *orientation;

	Settings::Length *drag_knife_blade_offset;	// Only relevant if the tool's type is eDragKnife.
	Settings::Double *drag_knife_initial_cutting_direction;	// At program start, the drag knife will be pointing ready to cut in this direction (degrees from positive X axis)


	

private:
	void ResetSettingsToReasonableValues(const bool suppress_warnings = false );


private:
	Settings::TPGSettings *settings;

	void ResetTitle();

public:
	static QString FractionalRepresentation( const Cam::Settings::Length *original_value, const int max_denominator = 64 );
	static QString PrintedCircuitBoardRepresentation( const Cam::Settings::Length *pDiameter );
	static QString GuageNumberRepresentation( const Cam::Settings::Length *pDiameter );

	ToolFeature::centre_drill_t *ToolFeature::CentreDrillDefinition( const QString size ) const;

	// Preferred settings access...
	double CuttingRadius( const Cam::Settings::Length::Units_t units, const Cam::Settings::Length length = Cam::Settings::Length(-1, Cam::Settings::Length::Metric) ) const;
	const eToolType ToolType() const;
	double Pitch( const Cam::Settings::Length::Units_t units ) const { return(this->pitch->get( units )); }
	eTappingDirection_t TappingDirection() const { return(eTappingDirection_t(direction->get().first)); }
	QString SetupInstructions() const { return(setup_instructions->getValue()); }
	QString Title() const { return(title->getValue()); }	
	double CornerRadius( const Cam::Settings::Length::Units_t units ) const { return(corner_radius->get(units)); }
	double FlatRadius( const Cam::Settings::Length::Units_t units ) const { return(flat_radius->get(units)); }
	double CuttingEdgeAngle() const { return(cutting_edge_angle->get()); }
	double CuttingEdgeHeight( const Cam::Settings::Length::Units_t units ) const { return(cutting_edge_height->get(units)); }
	double MaxAdvancePerRevolution( const Cam::Settings::Length::Units_t units ) const { return(max_advance_per_revolution->get(units)); }
	bool AutomaticallyGenerateTitle() const { return(automatically_generate_title->get().first != 0); }
	double ProbeOffsetX( const Cam::Settings::Length::Units_t units) const { return(probe_offset_x->get(units)); }
	double ProbeOffsetY( const Cam::Settings::Length::Units_t units) const { return(probe_offset_y->get(units)); }
	double Gradient( const Cam::Settings::Length::Units_t units ) const { return(gradient->get(units)); }
	double ToolLengthOffset( const Cam::Settings::Length::Units_t units ) const { return(tool_length_offset->get(units)); }
	eMaterial_t Material() const { return(eMaterial_t(material->get().first)); }
	eExtrusionMaterial_t ExtrusionMaterial() const { return(eExtrusionMaterial_t(extrusion_material->get().first)); }
	QString CentreDrillSize() const { return(this->centre_drill_size->get().second); }
	double XOffset(const Cam::Settings::Length::Units_t units) const { return(x_offset->get(units)); }
	double FrontAngle() const { return(front_angle->get()); }
	double ToolAngle() const { return(tool_angle->get()); }
	double BackAngle() const { return(back_angle->get()); }
	eOrientation_t Orientation() const { return(eOrientation_t(orientation->get().first)); }
	double DragKnifeBladeOffset( const Cam::Settings::Length::Units_t units ) const { return(drag_knife_blade_offset->get(units)); }
	double DragKnifeInitialCuttingDirection() const { return(drag_knife_initial_cutting_direction->get()); }

	bool SettingsInitialized() const;

public:
	static TopoDS_Shape StanleyKnifeBlade_MountingPlate(const double blade_angle_degrees, const double blade_thickness);
	static TopoDS_Shape StanleyKnifeBlade(const double blade_angle_degrees, const double blade_thickness);
	TopoDS_Shape Shape() const;

	// Define some helper functions to convert from the various enumerations into strings.
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

	friend std::ostringstream & operator << ( std::ostringstream & oss, const eMaterial_t & material_type )
	{
		QString qs;
		qs << material_type;
		oss << qs.toAscii().constData();
		return(oss);
	}


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

	friend std::ostringstream & operator<< ( std::ostringstream & oss, const eToolType & tool_type )
	{
		QString qs;
		qs << tool_type;
		oss << qs.toAscii().constData();
		return(oss);
	}

	friend QString & operator<< ( QString & qs, const eExtrusionMaterial_t & extrusion_material )
	{
		std::ostringstream ss;
		switch (extrusion_material)
		{
		case eABS:		ss << "ABS";
			break;

		case ePLA:	ss << "PLA";
			break;

		case eHDPE:	ss << "HDPE";
			break;

		case eUndefinedExtrusionMaterialType:	ss << "Undefined";
			break;
		} // End switch

		qs.append( QString::fromStdString(ss.str()) );
		return(qs);
	}

	friend std::ostringstream & operator<< ( std::ostringstream & oss, const eExtrusionMaterial_t & extrusion_material )
	{
		QString qs;
		qs << extrusion_material;
		oss << qs.toAscii().constData();
		return(oss);
	}

	
	friend QString & operator << ( QString & ss, const eTappingDirection_t & direction )
	{
		switch (direction)
		{
		case eRightHandThread:
			ss.append(QString::fromAscii("Right Hand"));
			break;

		case eLeftHandThread:
			ss.append(QString::fromAscii("Left Hand"));
			break;
		}

		return(ss);
	}

	friend std::ostringstream & operator<< ( std::ostringstream & oss, const eTappingDirection_t & direction )
	{
		QString qs;
		qs << direction;
		oss << qs.toAscii().constData();
		return(oss);
	}

	friend QString & operator << ( QString & ss, const eOrientation_t & orientation )
	{
		switch (orientation)
		{
		case eUnused:
			ss.append(QString::fromAscii("Unused"));
			break;

		case eTurningBackFacing:
			ss.append(QString::fromAscii("Turning/Back Facing"));
			break;

		case eTurningFacing:
			ss.append(QString::fromAscii("Turning Facing"));
			break;

		case eBoringFacing:
			ss.append(QString::fromAscii("Boring Facing"));
			break;

		case eBoringBackFacing:
			ss.append(QString::fromAscii("Boring/Back Facing"));
			break;

		case eTurning:
			ss.append(QString::fromAscii("Turning"));
			break;

		case eFacing:
			ss.append(QString::fromAscii("Facing"));
			break;

		case eBoring:
			ss.append(QString::fromAscii("Boring"));
			break;

		case eCentre:
			ss.append(QString::fromAscii("Centre"));
			break;
		}

		return(ss);
	}

	friend std::ostringstream & operator<< ( std::ostringstream & oss, const eOrientation_t & orientation )
	{
		QString qs;
		qs << orientation;
		oss << qs.toAscii().constData();
		return(oss);
	}

private:
	// Utility method.
	double degrees_to_radians( const double degrees ) const;
};

} //namespace Cam


#endif //CAM_TOOLFEATURE_H
