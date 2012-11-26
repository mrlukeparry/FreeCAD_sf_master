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

#include <TPG/CppTPGDescriptor.h>

#include "CppExampleTPG.h"

#define myID   "95744f1e-360f-11e2-bcd3-08002734b94f"
#define myName "Example CPP TPG"
#define myDesc "A simple example CPP TPG to demonstrating how to create one."

/**
 * Implement the Cpp shared library interface functions
 * Note: this must be outside the namespace declaration
 */
CPPTPG_API_SOURCE(Cam::CppExampleTPG, myID, myName, myDesc)

/// TPG Implementation ///
namespace Cam {

CppExampleTPG::CppExampleTPG() {
    id = QString::fromAscii(myID);
    name = QString::fromAscii(myName);
    description = QString::fromAscii(myDesc);
}

CppExampleTPG::~CppExampleTPG() {
}

/**
 * Run the TPG to generate the ToolPath code.
 *
 * Note: the return will change once the TP Language has been set in stone
 */
void TPG::run(TPGSettings *settings, QString action="")
{
    printf("This is where the TPG would generate the tool-path!\n");
    return;
}

} /* namespace Cam */
