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

#include <Mod/Cam2/App/MachineProgram.h>

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_increment_actor.hpp>

using namespace BOOST_SPIRIT_CLASSIC_NS;

/// Base class
namespace Cam
{

class CamExport GCode
{

public:
    GCode();
    ~GCode();

public:
	virtual rule<> rs274();

	virtual parse_info<> Parse( MachineProgram *pMachineProgram );

protected:
	/**
		Define boost::spirit::classic rules for each of the various 'tokens' that make up
		an RS274 GCode program.  Make them virtual so that this class can be overridden
		to make specific GCode parsers for different machine controllers.
	 */
	virtual rule<> line_number() { return ((chlit<>('N') | chlit<>('n')) >> uint_parser<>()); }

	virtual rule<> g00() { return ((chlit<>('G') | chlit<>('g')) >> (!(chlit<>('0')) >> chlit<>('0'))); }
	virtual rule<> g01() { return ((chlit<>('G') | chlit<>('g')) >> (!(chlit<>('0')) >> chlit<>('1'))); }
	virtual rule<> g02() { return ((chlit<>('G') | chlit<>('g')) >> (!(chlit<>('0')) >> chlit<>('2'))); }

	virtual rule<> end_of_block() { return eol_p; }

	// The work_offset rule changes between machine controllers so separate it out in its own rule.
	virtual rule<> work_offset() { return ((chlit<>('G') | chlit<>('g')) >> str_p<>("10") >> (chlit<>('L') | chlit<>('l')) >> str_p<>("20")); }

};

class CamExport LinuxCNC : public GCode
{
public:
	LinuxCNC();
	~LinuxCNC();

protected:
	// Overload the work_offset rule because LinuxCNC uses G10 L20 for that.
	virtual rule<> work_offset() { return ((chlit<>('G') | chlit<>('g')) >> str_p<>("10") >> (chlit<>('L') | chlit<>('l')) >> str_p<>("20")); }

}; // End LinuxCNC GCode parser class definition.

class CamExport Mach3 : public GCode
{
public:
	Mach3();
	~Mach3();

protected:
	// Overload the work_offset rule because LinuxCNC uses G10 L2 for that.
	virtual rule<> work_offset() { return ((chlit<>('G') | chlit<>('g')) >> str_p<>("10") >> (chlit<>('L') | chlit<>('l')) >> str_p<>('2')); }

}; // End LinuxCNC GCode parser class definition.

} //namespace Cam
#endif //CAM_GCODE_H
