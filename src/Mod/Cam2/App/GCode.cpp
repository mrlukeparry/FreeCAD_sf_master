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


#include <PreCompiled.h>
#ifndef _PreComp_
#endif

#include <sstream>
#include <math.h>

// NOTE: These BOOST SPIRIT DEBUG macros MUST be defined BEFORE the include
// files.  Otherwise the default values, defined within the include files,
// will be used instead.

#include "GCode.h"


using namespace Cam;

// Define an ostringstream to accumulate any debug output the grammar has
// to offer.  If we fail to parse the GCode, we will want to know why.
std::ostringstream CamExport GCode_GrammarDebugOutputBuffer;

GCode::GCode(MachineProgram *machine_program)
{
	required_decimal_places = 3;
	tolerance = 1.0 / pow(10.0, double(required_decimal_places));

	if (machine_program != NULL)
	{
		this->machine_program = machine_program->grab();	// Let the MachineProgram object know we're watching.

		// Get a reference to the ToolPath object so we can calculate the required tolerance.  Once calculated,
		// discard the ToolPath reference and keep the answer instead.
		ToolPath *toolPath = this->machine_program->getToolPath();
		this->required_decimal_places = toolPath->RequiredDecimalPlaces();
		this->tolerance = 1.0 / pow(10, double(toolPath->RequiredDecimalPlaces()));
		toolPath->release();
	}
	else
	{
		this->machine_program = NULL;	// Make sure we know we don't have a valid pointer.
	}
}

GCode::~GCode()
{
	if (this->machine_program != NULL)
	{
		this->machine_program->release();
		this->machine_program = NULL;
	}
}

void GCode::AddWarning(const QString warning)
{
	warnings << warning;
}

GCode::GraphicalReference::GraphicalReference(MachineProgram *machine_program)
{
	this->machine_program = machine_program->grab();
	this->part_feature = NULL;
}

GCode::GraphicalReference::~GraphicalReference()
{
	this->machine_program->release();
}

GCode::GraphicalReference::GraphicalReference( const GCode::GraphicalReference & rhs )
{
	if (this != &rhs)
	{
		this->machine_program = rhs.machine_program->grab();
		this->machine_program_index = rhs.machine_program_index;
		this->part_feature = rhs.part_feature;
	}
}

GCode::GraphicalReference & GCode::GraphicalReference::operator=( const GCode::GraphicalReference & rhs )
{
	if (this != &rhs)
	{
		if (this->machine_program) this->machine_program->release();
		this->machine_program = rhs.machine_program->grab();
		this->machine_program_index = rhs.machine_program_index;
		this->part_feature = rhs.part_feature;
	}

	return(*this);
}

