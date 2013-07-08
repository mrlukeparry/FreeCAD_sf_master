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
#include <TPG/CppTPGDescriptor.h>

#include "CppExampleTPG.h"
#include "Graphics/Paths.h"

#define myID   "95744f1e-360f-11e2-bcd3-08002734b94f"
#define myName "Example CPP TPG"
#define myDesc "A simple example CPP TPG to demonstrating how to create one. "



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
/* static */ QString CppExampleTPG::SettingName_Standoff = QString::fromAscii("standoff");
/* static */ QString CppExampleTPG::SettingName_Dwell = QString::fromAscii("dwell");
/* static */ QString CppExampleTPG::SettingName_PeckDepth = QString::fromAscii("peck_depth");
/* static */ QString CppExampleTPG::SettingName_RetractMode = QString::fromAscii("retract_mode");
/* static */ QString CppExampleTPG::SettingName_Clearance = QString::fromAscii("clearance_relative_height");

CppExampleTPG::CppExampleTPG()
        : CppTPG() { // important to call parent constructor
    id = QS(myID);
    name = QS(myName);
    description = QS(myDesc);

    QString qaction = QS("default");
    actions.push_back(qaction);
    settings = new TPGSettings();
    settings->addSettingDefinition(qaction, new TPGSettingDefinition("geometry", "Geometry", "Cam::TextBox", "Box01", "", "The input geometry that should be cut"));

	// settings->addSettingDefinition(qaction, new TPGSettingDefinition(SettingName_Depth.toAscii().constData(), "Geometry", "Cam::TextBox", "Box01", "", "The input geometry that should be cut"));
	settings->addSettingDefinition(qaction, new TPGSettingDefinition("Relative Depth", "Relative Depth", "Cam::TextBox", "10.0", "", "The input geometry that should be cut"));

	/*	
	settings->addSettingDefinition(qaction, new TPGSettingDefinition("Relative Depth", 
																	 "Relative Depth",
																	 "Cam::TextBox", 
																	 "10.0",
																	 "",
																	 "Distance from the current Z location to the bottom of the hole.  Must be positive"));
	*/

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
	if (this->toolpath == NULL)
	{
		qDebug("Releasing previously generated toolpath\n");
		this->toolpath->release();	// release the previous copy and generate a new one.
	}

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

	typedef enum
	{
		rapid_retract = 0,
		feed_retract
	} RetractMode_t;

	// TODO Get these parameters from the settings
	qDebug("action='%s'\n", action.toAscii().constData());
	qDebug("name='%s'\n", SettingName_Depth.toAscii().constData());

	QString str_depth = settings->getValue( action, SettingName_Depth );
	double depth = settings->getValue( action, SettingName_Depth ).toDouble() / units;
	double standoff = settings->getValue( action, SettingName_Standoff ).toDouble() / units;
	double dwell = settings->getValue( action, SettingName_Dwell ).toDouble();
	double peck_depth = settings->getValue( action, SettingName_PeckDepth ).toDouble() / units;
	int    retract_mode = settings->getValue( action, SettingName_RetractMode ).toInt();
	double clearance_height = settings->getValue( action, SettingName_Clearance ).toDouble() / units;

	// Mark the beginning and end of the GCode generated by this TPG just in case we want to find
	// it again from the whole GCode program (for highlighting etc.)
	QString tpg_reference = QString::fromAscii("TpgBegin ");
	tpg_reference += QString::fromAscii("id='") + this->getId() + QString::fromAscii("'");
	tpg_reference += QString::fromAscii(", name='") + this->getName() + QString::fromAscii("'");
	python << "comment(" << python.PythonString(tpg_reference) << ")\n";

	// Generate the drilling locations from the Paths objects.  i.e. points as well as intersecting sketches etc.
	Cam::Paths::Locations_t locations = paths.PointLocationData();
	for (Cam::Paths::Locations_t::iterator itLocation = locations.begin(); itLocation != locations.end(); itLocation++)
	{
		// gp_Pnt point = pMachineState->Fixture().Adjustment( *l_itLocation );
		gp_Pnt point = itLocation->Location();

		python	<< "drill("
				<< "x=" << point.X()/units << ", "
				<< "y=" << point.Y()/units << ", "
				<< "z=" << point.Z()/units << ", "
				<< "depth=" << depth << ", "
				<< "standoff=" << standoff << ", "
				<< "dwell=" << dwell << ", "
				<< "peck_depth=" << peck_depth << ", "
				<< "retract_mode=" << retract_mode << ", "
				<< "clearance_height=" << clearance_height
				<< ")\n";
        // pMachineState->Location(point); // Remember where we are.
	} // End for

	python << "end_canned_cycle()\n";


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
