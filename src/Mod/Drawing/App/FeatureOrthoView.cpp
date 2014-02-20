/***************************************************************************
 *   Copyright (c) 2014 Luke Parry <l.parry@warwick.ac.uk>                 *
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
# include <sstream>
#endif

#include <strstream>
#include <Base/Console.h>
#include <Base/Writer.h>

#include "FeatureOrthoView.h"

using namespace Drawing;

const char* FeatureOrthoView::TypeEnums[]= {"Front",
                                            "Left",
                                            "Right",
                                            "Back",
                                            "Top",
                                            "Bottom",
                                            NULL};

PROPERTY_SOURCE(Drawing::FeatureOrthoView, Drawing::FeatureViewPart)

FeatureOrthoView::FeatureOrthoView(void)
{
    Type.setEnums(TypeEnums);
    ADD_PROPERTY(Type,((long)0));

    // Set Hidden
    Direction.StatusBits.set(3);

    // Set Hidden
    XAxisDirection.StatusBits.set(3);

    // Scale is Readonly
    Scale.StatusBits.set(2);
}

short FeatureOrthoView::mustExecute() const
{
    if (Type.isTouched())
        return 1;
    return Drawing::FeatureViewPart::mustExecute();
}

/// get called by the container when a Property was changed
void FeatureOrthoView::onChanged(const App::Property* prop)
{
    if (prop == &X ||
        prop == &Y ||
        prop == &Type ||
        prop == &Scale){
          if (!this->isRestoring()) {
              this->touch();
          }
    }
    Drawing::FeatureViewPart::onChanged(prop);
}

FeatureOrthoView::~FeatureOrthoView()
{
}

void FeatureOrthoView::onDocumentRestored()
{
    // Rebuild the view
    this->execute();
}

App::DocumentObjectExecReturn *FeatureOrthoView::execute(void)
{

    std::string projType = this->Type.getValueAsString();

    if(strcmp(projType.c_str(), "Front") == 0) {
        Direction.setValue(0., 1., 0.);
        XAxisDirection.setValue(1., 0., 0.);
    } else if(strcmp(projType.c_str(), "Back") == 0) {
        Direction.setValue(0., -1., 0.);
        XAxisDirection.setValue(-1., 0., 0);
    } else if(strcmp(projType.c_str(), "Right") == 0) {
        Direction.setValue(1., 0., 0.);
        XAxisDirection.setValue(0, -1., 0);
    } else if(strcmp(projType.c_str(), "Left") == 0) {
        Direction.setValue(-1., 0., 0.);
        XAxisDirection.setValue(0, 1., 0);
    } else if(strcmp(projType.c_str(), "Top") == 0) {
        Direction.setValue(0., 0., 1.);
        XAxisDirection.setValue(1., 0., 0);
    } else if(strcmp(projType.c_str(), "Bottom") == 0) {
        Direction.setValue(0., 0., -1.);
        XAxisDirection.setValue(1., 0., 0);
    }

    return Drawing::FeatureViewPart::execute();
}

