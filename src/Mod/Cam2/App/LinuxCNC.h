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

#ifndef CAM_LINUX_CNC_H
#define CAM_LINUX_CNC_H

#include <PreCompiled.h>

#include <Mod/Cam2/App/GCode.h>


///////////////////////////////////////////////////////////////////////////////

/// Base class
namespace Cam
{

class CamExport LinuxCNC : public GCode
{
public:
    LinuxCNC();
    ~LinuxCNC();

public:
	virtual Geometry_t Parse(const char *program);

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

	/*
		NOTE: The order of the eStatement_t enumeration determines the 
		importance of each line of GCode.  Only the most 'important' (highest number)
		will affect both the colour and the NCCode object lines added as a result.

		eg: If a line of GCode includes;
		N50 G01 X1 Y2 Z3 (This is a movement)

		Then this line could be interpreted as a;
			- line number
			- feed rate movement
			- comment

		Of these, the feed movement (G01) is the most important so the stFeed statement
		type must appear AFTER the stComment value in this enumeration.
	 */
	typedef enum {
		stUndefined = 0,
		stProgramFlow,
		stIfStatement,
		stElseStatement,
		stEndifStatement,
		stWhileStatement,
		stEndWhileStatement,
		stComment,
		stProgram,
		stPreparation,
		stDataSetting,
		stAxis,
		stVariable,
		stG28,
		stG30,
		stG92,
		stG92_1,
		stG92_2,
		stG92_3,
		stToolLengthEnabled,
		stToolLengthDisabled,
		stToolChange,
		stProbe,
		stRapid,
		stFeed,
		stArcClockwise,
		stArcCounterClockwise,
		stDrilling,
		stTapping,
		stBoring
	} eStatement_t;

}; // End LinuxCNC class definition.


} //namespace Cam
#endif //CAM_LINUX_CNC_H
