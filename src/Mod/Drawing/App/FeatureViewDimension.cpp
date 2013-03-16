/***************************************************************************
 *   Copyright (c) 2013 Luke Parry <l.parry@warwick.ac.uk>                 *
 *                                                                         *
 *   This file is Drawing of the FreeCAD CAx development system.           *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A DrawingICULAR PURPOSE.  See the      *
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
# include <sstream>
#endif

# include <Mod/Measure/App/Measurement.h>

# include "FeatureViewDimension.h"

using namespace Drawing;

//===========================================================================
// FeatureViewDimension
//===========================================================================

PROPERTY_SOURCE(Drawing::FeatureViewDimension, Drawing::FeatureViewAnnotation)

const char* FeatureViewDimension::TypeEnums[]= {"Distance", "DistanceX", "DistanceY",NULL};

FeatureViewDimension::FeatureViewDimension(void) 
{
    ADD_PROPERTY_TYPE(References,(0,0),"Dimension",(App::PropertyType)(App::Prop_None),"Dimension Supporting References");
    Type.setEnums(TypeEnums);
    ADD_PROPERTY(Type,((long)0), "Dimension", (App::PropertyType)(App::Prop_None),"Dimension Type");
}

FeatureViewDimension::~FeatureViewDimension()
{
}

short FeatureViewDimension::mustExecute() const
{
    // If Tolerance Property is touched
    if(References.isTouched())
        return 1;
    else 
        return 0;
//     return Drawing::FeatureView::mustExecute();
}

App::DocumentObjectExecReturn *FeatureViewDimension::execute(void)
{
    //Relcalculate the measurement based on references stored. 
    Measure::Measurement measure;
    const std::vector<App::DocumentObject*> &objects = References.getValues();
    const std::vector<std::string> &subElements = References.getSubValues();
    
    std::vector<App::DocumentObject*>::const_iterator obj = objects.begin();
    std::vector<std::string>::const_iterator subEl = subElements.begin();
    
    for(; obj != objects.end(); ++obj, ++subEl) {
        measure.addReference(*obj, (*subEl).c_str());        
    }

    return App::DocumentObject::StdReturn;
}

