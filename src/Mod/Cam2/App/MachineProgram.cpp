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

#include <PreCompiled.h>
#ifndef _PreComp_
#endif

#include "MachineProgram.h"

namespace Cam {

MachineProgram::MachineProgram() {
    refcnt = 1;
    machineProgram = new QStringList();
	errors = new QStringList();
}

MachineProgram::~MachineProgram() {
    if (this->machineProgram != NULL)
	{
        delete this->machineProgram;
	}

	if (this->errors != NULL)
	{
		delete this->errors;
	}
}

/**
 * Add a single Machine command to the Program
 */
void MachineProgram::addMachineCommand(QString mc) {
    if (this->machineProgram == NULL)
        this->machineProgram = new QStringList();

	if (mc.endsWith(QString::fromAscii("\n"))) mc.remove(mc.size()-1, 1);	// Strip off the newline character.
    this->machineProgram->push_back(mc);
}

void MachineProgram::addErrorString(QString error_string) {
    if (this->errors == NULL)
        this->errors = new QStringList();

    this->errors->push_back(error_string);
}

/**
 * Clear out the toolpath.
 */
void MachineProgram::clear() {
    if (this->machineProgram != NULL)
        this->machineProgram->clear();
    else
        this->machineProgram = new QStringList();

	if (this->errors != NULL)
        this->errors->clear();
    else
        this->errors = new QStringList();
}

/**
 * Get the Machine Program as a list of strings
 */
QStringList *MachineProgram::getMachineProgram() {
    return machineProgram;
}

QStringList *MachineProgram::getErrors() {
	return this->errors;
}


/* friend */ QString operator<< ( QString & buf, const MachineProgram & machine_program )
{
	if (machine_program.machineProgram)
	{
		for (QStringList::const_iterator itString = machine_program.machineProgram->begin(); itString != machine_program.machineProgram->end(); itString++)
		{
			buf.append(*itString);
			buf.append(QString::fromAscii("\n"));
		}
	}
	return(buf);
}



} /* namespace Cam */
