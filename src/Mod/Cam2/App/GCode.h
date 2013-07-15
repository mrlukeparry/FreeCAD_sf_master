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

#include <boost/spirit/include/qi.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>


///////////////////////////////////////////////////////////////////////////////
int CamExport wilma();

/// Base class
namespace Cam
{

class CamExport GCode
{
public:
    GCode();
    ~GCode();

public:

}; // End GCode class definition.


/*
class CamExport LinuxCNC : public GCode
{
public:
	LinuxCNC();
	~LinuxCNC();

protected:
	// Overload the work_offset rule because LinuxCNC uses G10 L20 for that.
	virtual rule<> work_offset() { return ((chlit<>('G') | chlit<>('g')) >> str_p<>("10") >> (chlit<>('L') | chlit<>('l')) >> str_p<>("20")); }

	// Variables can be #9999 or #<alpha> in LinuxCNC
	virtual rule<> variable() { return (GCode::variable() | (str_p<>("#<") >> +(char_("a-zA-Z0-9_")) >> str_p<>(">")) ); }

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
*/

} //namespace Cam
#endif //CAM_GCODE_H
