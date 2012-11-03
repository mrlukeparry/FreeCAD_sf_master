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
#include "PreCompiled.h"

#ifndef _PreComp_
#endif

#include <Mod/Cam2/App/CamFeature.h>
#include <Mod/Cam2/App/StockGeometry.h>
#include <Mod/Cam2/App/CamPartsList.h>
#include <Mod/Cam2/App/TPGList.h>
#include <Mod/Cam2/App/GCodeFeature.h>
#include "ViewProviderCamFeature.h"

using namespace CamGui;

PROPERTY_SOURCE(CamGui::ViewProviderCamFeature, Gui::ViewProviderDocumentObject)

ViewProviderCamFeature::ViewProviderCamFeature()
{
}

ViewProviderCamFeature::~ViewProviderCamFeature()
{
}

std::vector<App::DocumentObject*> ViewProviderCamFeature::claimChildren(void)const
{
    // Collect any child fields and put this in the CamFeature tree
    std::vector<App::DocumentObject*> temp;
    Cam::CamFeature *feat = static_cast<Cam::CamFeature*>(getObject());
    temp.push_back(dynamic_cast<App::DocumentObject* >(feat->getStockGeometry()));
    temp.push_back(dynamic_cast<App::DocumentObject* >(feat->getPartsContainer()));
    temp.push_back(dynamic_cast<App::DocumentObject* >(feat->getTPGContainer()));
    temp.push_back(dynamic_cast<App::DocumentObject* >(feat->getGCodeFeature()));
    return temp;
}

Cam::CamFeature* ViewProviderCamFeature::getObject() const
{
    return dynamic_cast<Cam::CamFeature*>(pcObject);
}