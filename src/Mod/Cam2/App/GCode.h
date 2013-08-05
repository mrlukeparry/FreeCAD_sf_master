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

#ifndef CAM_GCODE_H
#define CAM_GCODE_H

#include <App/PropertyStandard.h>
#include <PreCompiled.h>

// NOTE: These BOOST SPIRIT DEBUG macros MUST be defined BEFORE the include
// files.  Otherwise the default values, defined within the include files,
// will be used instead.

#ifndef BOOST_SPIRIT_DEBUG
	#define BOOST_SPIRIT_DEBUG
	#define BOOST_SPIRIT_DEBUG_OUT GCode_GrammarDebugOutputBuffer
#endif

#include <Mod/Cam2/App/MachineProgram.h>
#include <Mod/Part/App/PartFeature.h>

extern std::ostringstream CamExport GCode_GrammarDebugOutputBuffer;

///////////////////////////////////////////////////////////////////////////////
/// Base class
namespace Cam
{

class CamExport GCode
{
public:
	typedef enum {
		csUndefined = -1,
		csG53 = 0,
		csG54,
		csG55,
		csG56,
		csG57,
		csG58,
		csG59,
		csG59_1,
		csG59_2,
		csG59_3,
		csG92,
		csG92_1
	} eCoordinateSystems_t;

	typedef enum {
		eXYPlane = 0,
		eXZPlane,
		eYZPlane
	} ePlane_t;

public:
    GCode(MachineProgram *machine_program);
    ~GCode();

public:
	typedef std::vector<Part::Feature *> Geometry_t;
	virtual bool Parse() = 0;
	void AddWarning(const QString warning);

public:
	MachineProgram *machine_program;
	QStringList	warnings;

	// Define a type representing the index into the QStringList contained within the MachineProgram object.
	// We will assign specific graphics to these for later reference.
	typedef QStringList::size_type MachineProgramIndex_t;

	/**
		Define a class that contains a single graphical element representing
		tool movements defined by the GCode contained in the MachineProgram object.
		We will end up with a list of such graphical elements.
	 */
	class GraphicalReference
	{
	public:
		GraphicalReference(MachineProgram *machine_program);
		~GraphicalReference();
		GraphicalReference( const GraphicalReference & rhs );
		GraphicalReference & operator= ( const GraphicalReference & rhs );

		void Index(const MachineProgramIndex_t index) { machine_program_index = index; }
		MachineProgramIndex_t Index() const { return(machine_program_index); }

		void PartFeature( Part::Feature *part_feature ) { this->part_feature = part_feature; }
		Part::Feature *PartFeature() const { return(this->part_feature); }

		void CoordinateSystem( const eCoordinateSystems_t value ) { this->coordinate_system = value; }
		eCoordinateSystems_t CoordinateSystem() const { return(this->coordinate_system); }

	private:
		MachineProgram *machine_program;
		MachineProgramIndex_t machine_program_index;
		Part::Feature *part_feature;	// graphical element - line, arc etc.
		eCoordinateSystems_t coordinate_system;
	};

	typedef std::map< MachineProgramIndex_t, GraphicalReference > Graphics_t;

}; // End GCode class definition.


} //namespace Cam
#endif //CAM_GCODE_H
