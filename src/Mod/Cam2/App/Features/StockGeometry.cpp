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

#include "../PreCompiled.h"
#ifndef _PreComp_
#endif

#include <Base/Placement.h>
#include "StockGeometry.h"

using namespace Cam;

PROPERTY_SOURCE(Cam::StockGeometry, App::DocumentObject)

const char *stockGeomGroup = "Stock Geometry Feature";

StockGeometry::StockGeometry()
{
    Base::Placement place = Base::Placement();
//     ADD_PROPERTY_TYPE(Base, place , stockGeomGroup, (App::PropertyType)(App::Prop_Output) ,"Origin Placement");
    ADD_PROPERTY_TYPE(Geometry,(0), stockGeomGroup, (App::PropertyType)(App::Prop_None) ,"Stock Geometry");
}

StockGeometry::~StockGeometry()
{

}

short StockGeometry::mustExecute() const
{
    if (Geometry.isTouched() ||
        Base.isTouched())
        return 1;
    return 0;
}

// void StockGeometry::positionByBase(void)
// {
//     Part::Feature *Geometry = static_cast<Part::Feature*>(Geometry.getValue());
//     if (Geometry && Geometry->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId()))
//         this->Geometry.setValue(Geometry->Placement.getValue());
// }


App::DocumentObjectExecReturn *StockGeometry::execute(void)
{
    this->touch();
    return App::DocumentObject::StdReturn;
}

void StockGeometry::onChanged(const App::Property* prop)
{
    if (prop == &Geometry) {
    }

    App::DocumentObject::onChanged(prop);
}
