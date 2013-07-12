/***************************************************************************
 *   Copyright (c) 2012 Andrew Robinson <andrewjrobinson@gmail.com>        *
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

#include "PreCompiled.h"
#ifndef _PreComp_
#endif

#include <Base/Interpreter.h>		// For Python runtime
#include <App/PropertyUnits.h>
#include <TPG/CppTPGDescriptor.h>
#include <Mod/Part/App/PrimitiveFeature.h>

#include "CppExampleTPG.h"
#include "Graphics/Paths.h"
#include <Mod/Cam2/App/GCode.h>

#define myID   "95744f1e-360f-11e2-bcd3-08002734b94f"
#define myName "Example CPP TPG"
#define myDesc "A simple example CPP TPG to demonstrating how to create one. "


#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_increment_actor.hpp>

using namespace BOOST_SPIRIT_CLASSIC_NS;

/**
 * Implement the Cpp shared library interface functions
 * Note: this must be outside the namespace declaration
 */
//CPPTPG_API_SOURCE(Cam::CppExampleTPG, myID, myName, myDesc)

//TODO: put this back once the memory corruption in getTPG() is located.
extern "C" Q_DECL_EXPORT Cam::TPGDescriptorCollection* getDescriptors() {
    Cam::TPGDescriptorCollection* descriptors = new Cam::TPGDescriptorCollection();
    Cam::TPGDescriptor *descriptor = new Cam::CppTPGDescriptor(QString::fromAscii("95744f1e-360f-11e2-bcd3-08002734b94f"), QString::fromAscii("Example CPP TPG"), QString::fromAscii("A simple example CPP TPG to demonstrating how to create one. "));
    descriptors->add(descriptor);
    descriptor->release();
    return descriptors;
}
extern "C" Q_DECL_EXPORT Cam::CppTPG* getTPG(QString id) {
    if (id.compare(QString::fromAscii(myID)) == 0)
        return new Cam::CppExampleTPG();
    return 0;
}

/// TPG Implementation ///
namespace Cam {

// Define the names used for this TPG's settings once here to avoid inconsistencies.
/* static */ QString CppExampleTPG::SettingName_Depth = QString::fromAscii("Relative Depth");
/* static */ QString CppExampleTPG::SettingName_Standoff = QString::fromAscii("Standoff");
/* static */ QString CppExampleTPG::SettingName_Dwell = QString::fromAscii("Dwell");
/* static */ QString CppExampleTPG::SettingName_PeckDepth = QString::fromAscii("Peck Depth");
/* static */ QString CppExampleTPG::SettingName_RetractMode = QString::fromAscii("Retract Mode");
/* static */ QString CppExampleTPG::SettingName_Clearance = QString::fromAscii("Clearance Relative Height");
/* static */ QString CppExampleTPG::SettingName_SpindleSpeed = QString::fromAscii("Spindle Speed");
/* static */ QString CppExampleTPG::SettingName_FeedRate = QString::fromAscii("Feed Rate");


/* friend */ QString & operator<< ( QString & buf, const CppExampleTPG::RetractMode_t & retract_mode )
{
	switch (retract_mode)
	{
	case CppExampleTPG::eRapidRetract:
		buf += QString::fromAscii("Rapid");
		return(buf);

	case CppExampleTPG::eFeedRetract:
		buf += QString::fromAscii("At Feed Rate");
		return(buf);
	} // End switch

	return(buf);
}


/**
	Itterate through the various RetractMode_t values and use the operator<<(QString) method
	to generate the text string for each value in turn.  That way we have a single place in
	the code that associates the RetractMode_t enumerated value to the QString representation.
 */
CppExampleTPG::RetractMode_t CppExampleTPG::toRetractMode( const QString string_representation ) const
{
	for (RetractMode_t mode = eRapidRetract; mode <= eFeedRetract; mode = RetractMode_t(int(mode)+1))
	{
		QString str;
		str << mode;
		if (str == string_representation) return(mode);
	}

	return(eRapidRetract);	// Should never get here.
}


CppExampleTPG::CppExampleTPG()
        : CppTPG() { // important to call parent constructor
    id = QS(myID);
    name = QS(myName);
    description = QS(myDesc);

	this->toolpath = NULL;

    QString qaction = QS("default");
    actions.push_back(qaction);

    settings = new TPGSettings();
    settings->addSettingDefinition(qaction, new TPGSettingDefinition("geometry", "Geometry", "Cam::TextBox", "Box01", "", "The input geometry that should be cut"));

	// TODO - Figure out how to use a 'length' property so that these values can be expressed in whatever units are configured
	// for use.
	settings->addSettingDefinition(qaction, new TPGSettingDefinition(SettingName_Depth.toAscii().constData(), 
																	 SettingName_Depth.toAscii().constData(),
																	 "Cam::TextBox", 
																	 "10.0",
																	 "mm",
																	 "Distance from the current Z location to the bottom of the hole.  Must be positive"));

	settings->addSettingDefinition(qaction, new TPGSettingDefinition(SettingName_Standoff.toAscii().constData(), 
																	 SettingName_Standoff.toAscii().constData(),
																	 "Cam::TextBox", 
																	 "5.0",
																	 "mm",
																	 "Distance above the drilling point location to retract to following the drilling cycle."));

	settings->addSettingDefinition(qaction, new TPGSettingDefinition(SettingName_Dwell.toAscii().constData(), 
																	 SettingName_Dwell.toAscii().constData(),
																	 "Cam::TextBox", 
																	 "0.0",
																	 "seconds",
																	 "Time (in seconds) for which the machine pauses at the bottom of a drilling cycle to break 'stringers'"));

	settings->addSettingDefinition(qaction, new TPGSettingDefinition(SettingName_PeckDepth.toAscii().constData(), 
																	 SettingName_PeckDepth.toAscii().constData(),
																	 "Cam::TextBox", 
																	 "5.0",
																	 "mm",
																	 "Distance used for itterative movements down into the hole with retractions between each.  If this is zero then peck drilling is disabled."));

	// TODO - Figure out how to express enumerated types as a property.  i.e. convert the
	// enumeration to a series of strings and present them as a 'combo-box' of drop-down options.
	QString retract_mode;
	retract_mode << eRapidRetract;	// Use the conversion method to retrieve the string used for retraction.
	settings->addSettingDefinition(qaction, new TPGSettingDefinition(SettingName_RetractMode.toAscii().constData(), 
																	 SettingName_RetractMode.toAscii().constData(),
																	 "Cam::TextBox", 
																	 retract_mode.toAscii().constData(),
																	 "mode",
																	 "0 represents a rapid ratract movement.  1 represents a retraction at the current feed rate."));

	settings->addSettingDefinition(qaction, new TPGSettingDefinition(SettingName_Clearance.toAscii().constData(), 
																	 SettingName_Clearance.toAscii().constData(),
																	 "Cam::TextBox", 
																	 "30.0",
																	 "mm",
																	 "Relative distance in Z to move to between holes to ensure the tool does not interfere with fixtures or other parts of the workpiece."));

	settings->addSettingDefinition(qaction, new TPGSettingDefinition(SettingName_SpindleSpeed.toAscii().constData(), 
																	 SettingName_SpindleSpeed.toAscii().constData(),
																	 "Cam::TextBox", 
																	 "700",
																	 "RPM",
																	 "Spindle Speed."));

	settings->addSettingDefinition(qaction, new TPGSettingDefinition(SettingName_FeedRate.toAscii().constData(), 
																	 SettingName_FeedRate.toAscii().constData(),
																	 "Cam::TextBox", 
																	 "55",
																	 "mm/min",
																	 "Feed Rate."));

    settings->addSettingDefinition(qaction, new TPGSettingDefinition("tool", "Tool", "Cam::TextBox", "Tool01", "", "The tool to use for cutting"));
    TPGSettingDefinition* speed = settings->addSettingDefinition(qaction, new TPGSettingDefinition("speed", "Speed", "Cam::Radio", "normal", "", "The speed of the algorithm.  Faster will use less accurate algorithm."));
    speed->addOption("fast", "Fast");
    speed->addOption("normal", "Normal");
    speed->addOption("slow", "Slow");
}

CppExampleTPG::~CppExampleTPG() {
    
}




/**
 * Run the TPG to generate the ToolPath code.
 *
 * Note: the return will change once the TP Language has been set in stone
 */
void CppExampleTPG::run(TPGSettings *settings, QString action= QString::fromAscii(""))
{
    qDebug("This is where the TPG would generate the tool-path! \n");
	if (this->toolpath != NULL)
	{
		qDebug("Releasing previously generated toolpath\n");
		this->toolpath->release();	// release the previous copy and generate a new one.
	}

	rule<> line_number = ((chlit<>('N') | chlit<>('n')) >> uint_parser<>());
	rule<> g00 = ((chlit<>('G') | chlit<>('g')) >> (!(chlit<>('0')) >> chlit<>('0')));
	rule<> g01 = ((chlit<>('G') | chlit<>('g')) >> (!(chlit<>('0')) >> chlit<>('1')));
	rule<> g02 = ((chlit<>('G') | chlit<>('g')) >> (!(chlit<>('0')) >> chlit<>('2')));

	rule<> end_of_block = eol_p;

	rule<> rs274 =	line_number >> end_of_block |
					line_number >> g00 >> end_of_block |
					line_number >> g01 >> end_of_block ;

	std::string gcode = "N300 G00\n";
	bool worked_ok = parse(gcode.c_str(), rs274, boost::spirit::classic::space_p).full;

	// Now generate a new toolpath.
	this->toolpath = new ToolPath(this);
	ToolPath &python = *(this->toolpath);	// for readability only.

	// TODO We really need to define which machine post processor is used at the CamFeature
	// level (or above would be better)  We DO NOT want this import to remain in the TPG itself.
	python << "from nc import *" << "\n";
	python << "import hm50" << "\n";
	
	// TODO: Understand how 'units' are handled in FreeCAD.
	// In HeeksCNC, we always store values in millimeters (mm) and convert to whatever external
	// units are configured at the last moment.  We store a 'units' value as 1.0 for mm and
	// 25.4 for inches.  This way, we always divide the internal representation by the
	// units value to produce a number suitable for the 'external' world.
	// When I say 'external world' I mean either presentation as a 'setting' so the operator
	// can change it or as a value being written to the Python program for GCode generation
	// (i.e. a ToolPath in the Cam workbench)
	// When the operator changes the units from one value to another, we need to change
	// the ToolPath::RequiredDecimalPlaces() value to either 3 (for metric) or 4 (for
	// imperial) so that the Python/GCode is generated using the correct resolution.

	python.RequiredDecimalPlaces(3);	// assume metric.
	python << "print 'hello world\\n'\n";

	python << "rapid(x=" << 12.3456789 << ")\n";
	python << "feed(x=" << 4 << ")\n";

	#ifdef FC_DEBUG
		// Run the unit tests for the ToolPath class.  This is for debug only.
		ToolPath::Test test(this->toolpath);
		if (test.Run())
		{
			python << "comment(" << python.PythonString("Tests for ToolPath succeeded") << ")\n";
		}
		else
		{
			python << "comment(" << python.PythonString("Tests for ToolPath failed") << ")\n";
		}
	#endif // FC_DEBUG

	python << "feed(x=" << 1.23456789 << ")\n";
	python << "comment(" << python.PythonString("we should be at x=1.235") << ")\n";

	python << "feed(x=" << 1.2355 << ")\n";
	python << "comment(" << python.PythonString("we should be at x=1.236") << ")\n";

	python << "feed(x=" << 1.2354 << ")\n";
	python << "comment(" << python.PythonString("we should be at x=1.235") << ")\n";

	python << "feed(x=" << -1.2355 << ")\n";
	python << "comment(" << python.PythonString("we should be at x=-1.236") << ")\n";

	python << "feed(x=" << -1.2354 << ")\n";
	python << "comment(" << python.PythonString("we should be at x=-1.235") << ")\n";

	QString some_setting(QString::fromUtf8("'Something (that) needs cleaning up befor it's included in a python comment() call'"));
	python << "comment(" << python.PythonString(some_setting) << ")\n";

	python << "comment(" << python.PythonString("the cat's mat has a single quote in it") << ")\n";
	python << "comment(" << python.PythonString("this one has (something in brackets) within it.") << ")\n";
	python << "comment(" << python.PythonString("'this one already has single quotes bounding it'") << ")\n";
	python << "comment(" << python.PythonString("\"this one already has double quotes bounding it\"") << ")\n";

	// Define a couple of intersecting lines just to play with the drilling cycle code.  This is
	// really to test the calling of the Python code held in the ToolPath through to the GCode
	// held in the MachineProgram.

	Cam::Paths paths;
	paths.Add( Cam::Edge( Cam::Point(0.0, 0.0, 0.0), Cam::Point(100.0, 0.0, 0.0) ) );
	paths.Add( Cam::Edge( Cam::Point(50.0, 0.0, 0.0), Cam::Point(50.0, 100.0, 0.0) ) );

	double units = 1.0;	// TODO Get some setting that indicates metric or imperial units for GCode.
	// For now, figure out the units by looking at the resolution of the Python code.
	if (python.RequiredDecimalPlaces() == 3)
	{
		units = 1.0;	// metric.
	}
	else
	{
		units = 25.4;	// imperial
	}

	// TODO - this arrangement with settings isn't going to work as it is.  We need to be able
	// to include enumerated types, register callback routines and also know what units
	// the values are in.  I've divided by a units value here on the assumption that the value
	// returned by the getValue() method is always going to be in mm.

	// Retrieve the various settings and convert them to the appropriate units for inclusion in the ToolPath.
	double depth = settings->getValue( action, SettingName_Depth ).toDouble() / units;
	double standoff = settings->getValue( action, SettingName_Standoff ).toDouble() / units;
	double dwell = settings->getValue( action, SettingName_Dwell ).toDouble();
	double peck_depth = settings->getValue( action, SettingName_PeckDepth ).toDouble() / units;
	RetractMode_t retract_mode = toRetractMode(settings->getValue( action, SettingName_RetractMode ));
	double clearance_height = settings->getValue( action, SettingName_Clearance ).toDouble() / units;
	double spindle_speed = settings->getValue( action, SettingName_SpindleSpeed ).toDouble() / units;
	double feed_rate = settings->getValue( action, SettingName_FeedRate ).toDouble() / units;

	// Mark the beginning and end of the GCode generated by this TPG just in case we want to find
	// it again from the whole GCode program (for highlighting etc.)
	QString tpg_reference = QString::fromAscii("TpgBegin ");
	tpg_reference += QString::fromAscii("id='") + this->getId() + QString::fromAscii("'");
	tpg_reference += QString::fromAscii(", name='") + this->getName() + QString::fromAscii("'");
	python << "comment(" << python.PythonString(tpg_reference) << ")\n";

	// Add some 'modal' values for the remainder of this machine operation's use.
	// TODO - select the appropriate fixture
	// TODO - select the appropriate tool

	python << "spindle(s=" << spindle_speed << ", clockwise=True)\n";
	python << "feedrate(" << feed_rate << ")\n";	

	// Generate the drilling locations from the Paths objects.  i.e. points as well as intersecting sketches etc.
	Cam::Paths::Locations_t locations = paths.PointLocationData();
	for (Cam::Paths::Locations_t::iterator itLocation = locations.begin(); itLocation != locations.end(); itLocation++)
	{
		// Align this point with the fixture (i.e. translation and/or rotation) so that the fixture's coordinate
		// system in the graphics file translates to the X,Y,Z coordinate system on the physical CNC machine.

		// gp_Pnt point = pMachineState->Fixture().Adjustment( l_itLocation->Location() );

		gp_Pnt point = itLocation->Location();	// Just for now...

		python	<< "drill("
				<< "x=" << point.X()/units << ", "
				<< "y=" << point.Y()/units << ", "
				<< "z=" << point.Z()/units << ", "
				<< "depth=" << depth << ", "
				<< "standoff=" << standoff << ", "
				<< "dwell=" << dwell << ", "
				<< "peck_depth=" << peck_depth << ", "
				<< "retract_mode=" << int(retract_mode) << ", "
				<< "clearance_height=" << clearance_height
				<< ")\n";
        // pMachineState->Location(point); // Remember where we are.
	} // End for

	python << "end_canned_cycle()\n";

	wilma();

	GCode a;
	a.fred();

	/*
	// Generate some graphics, convert it into the libArea representation and generate
	// the Python equivalent of that.
	Cam::Paths graphics;

	// Define a square
	graphics.Add( Cam::Edge( Cam::Point(0.0, 0.0, 0.0), Cam::Point(100.0, 0.0, 0.0)));
	graphics.Add( Cam::Edge( Cam::Point(100.0, 0.0, 0.0), Cam::Point(100.0, 100.0, 0.0)));
	graphics.Add( Cam::Edge( Cam::Point(100.0, 100.0, 0.0), Cam::Point(0.0, 100.0, 0.0)));
	graphics.Add( Cam::Edge( Cam::Point(0.0, 100.0, 0.0), Cam::Point(0.0, 0.0, 0.0)));

	// And add an arc into the middle of it.
	gp_Circ circle;
	gp_Pnt	centre(50.0, 50.0, 0.0);
	circle.SetLocation(centre);
	circle.SetAxis(gp_Ax1(centre, gp_Dir(gp_XYZ(0.0, 0.0, 1.0))));
	circle.SetRadius(10.0);
	graphics.Add( Cam::Edge(Cam::Point(centre.X() - circle.Radius(), centre.Y(), 0.0), Cam::Point(centre.X() + circle.Radius(), centre.Y(), 0.0), circle));

	python << "import sys\n";
	// python << "sys.path.insert(0, 'C:\\\\David\\\\src\\\\FreeCAD_sf_master\\\\VC9\\\\src\\Mod\\\\Cam2\\\\App\\\\Libs\\\\libarea-clipper')\n";
	python << "sys.path.insert(0, 'C:\\\\David\\\\src\\\\jdcnc\\\\trunk\\\\heekscnc')\n";
	python << "sys.path.insert(0, 'C:\\\\David\\\\src\\\\jdcnc\\\\trunk\\\\heekscnc\\\\Boolean')\n";
           	                       
	python << "import AreaClipper_d as area\n";
	// python << "import area\n";
	python << "import kurve_funcs as kurve_funcs\n";

	python << "comment('tool change to 1/8 inch End Mill')\n";
	python << "#tool_change( id=1)\n";
	python << "spindle(7000)\n";
	python << "feedrate_hv(6889.76378, 7)\n";
	python << "flush_nc()\n";
	python << "clearance = float(1.25)\n";
	python << "rapid_safety_space = float(0.07874015748)\n";
	python << "start_depth = float(0)\n";
	python << "step_down = float(0.035)\n";
	python << "final_depth = float(-0.07)\n";
	python << "tool_diameter = float(0.125)\n";
	python << "cutting_edge_angle = float(0)\n";

	python << "roll_radius = float(0)\n";
	python << "offset_extra = 0\n";

	// Convert the graphics into their Python representation so that the kurve_funcs Python script
	// can receive them in the format it expects.
	// This addAreaDefinition() adds an Area object and includes all the Curve objects within it.
	python.addAreaDefinition( graphics.Area(), "area_description" );

	// This addCurveDefinition() ONLY adds the Curve object represenations.  We do NOT need BOTH
	// the area and the curve represenation normally.  The area call above is really just so we
	// can see what the Area represenation looks like - for debugging.
	const char *l_pszObjectName = "outer_profile";
	python.addCurveDefinition( graphics[0].AreaCurve(), l_pszObjectName );

	// And back to the example taken from test.py
	python << "kurve_funcs.make_smaller( " << l_pszObjectName << ", start = area.Point(1.8, 0))\n";
	python << "roll_on = 'auto'\n";
	python << "roll_off = 'auto'\n";
	python << "extend_at_start= 0\n";
	python << "extend_at_end= 0\n";
	python << "lead_in_line_len= 0\n";
	python << "lead_out_line_len= 0\n";
	python << "kurve_funcs.profile(outer_profile, 'left', tool_diameter/2, offset_extra, roll_radius, roll_on, roll_off, rapid_safety_space, clearance, start_depth, step_down, final_depth,extend_at_start,extend_at_end,lead_in_line_len,lead_out_line_len )\n";
	python << "absolute()\n";
	python << "program_end()\n";
	*/

	// Mark the end of the GCode generated by this TPG just in case we want to find
	// it again from the whole GCode program (for highlighting etc.)
	tpg_reference.replace(QString::fromAscii("TpgBegin"), QString::fromAscii("TpgEnd"));
	python << "comment(" << python.PythonString(tpg_reference) << ")\n";
}


/* virtual */ ToolPath *CppExampleTPG::getToolPath()
{
	if (this->toolpath)
	{
		return(this->toolpath->grab());
	}
	else
	{
		return(NULL);
	}
}


} /* namespace Cam */
