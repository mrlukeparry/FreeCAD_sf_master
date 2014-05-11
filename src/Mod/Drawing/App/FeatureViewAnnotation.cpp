/***************************************************************************
 *   Copyright (c) 2013 Luke Parry <l.parry@warwick.ac.uk>                 *
 *   Copyright (c) 2012 Yorik van Havre <yorik@uncreated.net>              *
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

#include "FeatureViewAnnotation.h"

using namespace Drawing;

PROPERTY_SOURCE(Drawing::FeatureViewAnnotation, Drawing::FeatureView)

FeatureViewAnnotation::FeatureViewAnnotation(void)
{
    static const char *vgroup = "Drawing annotation";

    ADD_PROPERTY_TYPE(Font ,("osifont")         ,vgroup,App::Prop_None, "The name of the font to use");
    ADD_PROPERTY_TYPE(TextColor,(0.0f,0.0f,0.0f),vgroup,App::Prop_None, "The color of the text");

    Scale.StatusBits.set(3);
    ScaleType.StatusBits.set(3);
}

FeatureViewAnnotation::~FeatureViewAnnotation()
{
}

App::DocumentObjectExecReturn *FeatureViewAnnotation::execute(void)
{
    return App::DocumentObject::StdReturn;
}
