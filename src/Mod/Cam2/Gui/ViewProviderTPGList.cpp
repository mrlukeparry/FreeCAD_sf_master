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

#include "PreCompiled.h"

#ifndef _PreComp_
#endif

#include <Mod/Cam2/App/TPGList.h>
#include "ViewProviderTPGList.h"

using namespace CamGui;

PROPERTY_SOURCE(CamGui::ViewProviderTPGList, Gui::ViewProviderDocumentObject)

ViewProviderTPGList::ViewProviderTPGList()
{
}

ViewProviderTPGList::~ViewProviderTPGList()
{
}

std::vector<App::DocumentObject*> ViewProviderTPGList::claimChildren(void)const
{
    // Collect all the TPG Features which are contained in the list
    // These should be ordered visually by the way they are stored within the TPGObject Property
    std::vector<App::DocumentObject*> temp(static_cast<Cam::TPGList*>(getObject())->TPGObjects.getValues());
    return temp;
}

Cam::TPGList* ViewProviderTPGList::getObject() const
{
    return dynamic_cast<Cam::TPGList*>(pcObject);
}
