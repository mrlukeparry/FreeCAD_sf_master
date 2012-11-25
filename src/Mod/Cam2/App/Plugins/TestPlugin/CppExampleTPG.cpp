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

#include "../../PreCompiled.h"
#ifndef _PreComp_
#endif

#include "CppExampleTPG.h"
#include "../../TPG/CppTPGDescriptor.h"

/// Library functions ///
// Descriptors
extern "C" std::vector<Cam::TPGDescriptor*>* getDescriptors() {
    std::vector<Cam::TPGDescriptor*>* descriptors = new std::vector<Cam::TPGDescriptor*>();
    descriptors->push_back(new Cam::CppTPGDescriptor(QString::fromAscii("95744f1e-360f-11e2-bcd3-08002734b94f"),
            QString::fromAscii("Example CPP TPG"),
            QString::fromAscii("A simple example CPP TPG to demonstrating how to create one.")));
    return descriptors;
}
extern "C" void delDescriptors(std::vector<Cam::TPGDescriptor*>* descriptors) {
    std::vector<Cam::TPGDescriptor*>::iterator itt = descriptors->begin();
    for (;itt != descriptors->end(); ++itt)
        delete *itt;
    descriptors->clear();
    delete descriptors;
}

// TPGs
extern "C" Cam::TPG* getTPG(QString id) {
    if (id == QString::fromAscii("95744f1e-360f-11e2-bcd3-08002734b94f"))
        return new Cam::CppExampleTPG();
    return NULL;
}
extern "C" void delTPG(Cam::TPG* tpg) {
    if (tpg != NULL)
        delete tpg;
}


/// TPG Implementation ///
namespace Cam {

CppExampleTPG::CppExampleTPG() {
    id = QString::fromAscii("95744f1e-360f-11e2-bcd3-08002734b94f");
    name = QString::fromAscii("Example CPP TPG");
    description = QString::fromAscii("A simple example CPP TPG to demonstrating how to create one.");
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
