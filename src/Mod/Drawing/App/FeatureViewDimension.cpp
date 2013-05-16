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
# include <cstring>
#endif

# include <Base/Exception.h>
# include <Mod/Measure/App/Measurement.h>

# include "FeatureViewPart.h"
# include "FeatureViewDimension.h"

using namespace Drawing;

//===========================================================================
// FeatureViewDimension
//===========================================================================

PROPERTY_SOURCE(Drawing::FeatureViewDimension, Drawing::FeatureViewAnnotation)

const char* FeatureViewDimension::TypeEnums[]= {"Distance",
                                                "DistanceX",
                                                "DistanceY",
                                                "DistanceZ",
                                                "Radius",
                                                "Diameter",
//                                                 "Angle",
                                                NULL};

const char* FeatureViewDimension::ProjTypeEnums[]= {"True",
                                                    "Projected",
                                                    NULL};

FeatureViewDimension::FeatureViewDimension(void)
{
    ADD_PROPERTY_TYPE(References,(0,0),"Dimension",(App::PropertyType)(App::Prop_None),"Dimension Supporting References");
    ADD_PROPERTY_TYPE(Precision,(2)   ,"Dimension",(App::PropertyType)(App::Prop_None),"Dimension Precision");
    ADD_PROPERTY_TYPE(Fontsize,(6)    ,"Dimension",(App::PropertyType)(App::Prop_None),"Dimension Font Size");
    Type.setEnums(TypeEnums);
    ADD_PROPERTY(Type,((long)0));

    ProjectionType.setEnums(ProjTypeEnums);
    ADD_PROPERTY(ProjectionType,((long)0));

    this->measurement = new Measure::Measurement();
}

FeatureViewDimension::~FeatureViewDimension()
{
    delete measurement;
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
    //Clear the previous measurement made
    measurement->clear();

    //Relcalculate the measurement based on references stored.
    const std::vector<App::DocumentObject*> &objects = References.getValues();
    const std::vector<std::string> &subElements = References.getSubValues();

    std::vector<App::DocumentObject*>::const_iterator obj = objects.begin();
    std::vector<std::string>::const_iterator subEl = subElements.begin();

    for(; obj != objects.end(); ++obj, ++subEl) {
        Drawing::FeatureViewPart *viewPart = dynamic_cast<Drawing::FeatureViewPart *>(*obj);
        App::DocumentObject *docObj = viewPart->Source.getValue();

        if((*subEl).substr(0,4) == "Edge") {
            int idx = std::atoi((*subEl).substr(4,4000).c_str());
            char buff[100];
            sprintf(buff, "Edge%i", idx);
            measurement->addReference(docObj, buff);
        } else if((*subEl).substr(0,6) == "Vertex") {
            int idx = std::atoi((*subEl).substr(6,4000).c_str());
            char buff[100];
            sprintf(buff, "Vertex%i", idx);
            measurement->addReference(docObj, buff);
        }
    }

    return App::DocumentObject::StdReturn;
}

double FeatureViewDimension::getValue() const
{
    if(strcmp(Type.getValueAsString(), "Distance") == 0) {
        return measurement->length();
    } else if(strcmp(Type.getValueAsString(), "DistanceX") == 0){
        Base::Vector3d delta = measurement->delta();
        return delta.x;
    } else if(strcmp(Type.getValueAsString(), "DistanceY") == 0){
        Base::Vector3d delta = measurement->delta();
        return delta.y;
    } else if(strcmp(Type.getValueAsString(), "DistanceZ") == 0){
        Base::Vector3d delta = measurement->delta();
        return delta.z;
    } else if(strcmp(Type.getValueAsString(), "Radius") == 0){
        return measurement->radius();
    } else if(strcmp(Type.getValueAsString(), "Diameter") == 0){
        return measurement->radius() * 2;
    } else if(strcmp(Type.getValueAsString(), "Angle") == 0){
        return measurement->angle();
    }
    throw Base::Exception("Dimension Value couldn't be calculated");
}

