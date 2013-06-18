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
}

MachineProgram::~MachineProgram() {
    if (this->machineProgram == NULL)
        delete this->machineProgram;
}

/**
 * Add a single Machine command to the Program
 */
void MachineProgram::addMachineCommand(QString mc) {
    if (this->machineProgram == NULL)
        this->machineProgram = new QStringList();
    this->machineProgram->push_back(mc);
}

/**
 * Clear out the toolpath.
 */
void MachineProgram::clear() {
    if (this->machineProgram != NULL)
        this->machineProgram->clear();
    else
        this->machineProgram = new QStringList();
}

/**
 * Get the Machine Program as a list of strings
 */
QStringList *MachineProgram::getMachineProgram() {
    return machineProgram;
}

} /* namespace Cam */
