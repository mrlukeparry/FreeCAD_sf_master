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

#include "GCode.h"
using namespace Cam;
GCode::GCode()
{
}

GCode::~GCode()
{
}

/* virtual */ parse_info<> GCode::Parse( MachineProgram *pMachineProgram )
{
	QString program;
	program << *pMachineProgram;
	return(parse(program.toAscii().constData(), rs274(), space_p));
}

/* virtual */ rule<> GCode::rs274()
{
	rule<> rules =	(line_number() >> end_of_block()) [ qDebug("%s\n", "David is a good bloke"); ] |
					line_number() >> g00() >> end_of_block();

	return(rules);
}
