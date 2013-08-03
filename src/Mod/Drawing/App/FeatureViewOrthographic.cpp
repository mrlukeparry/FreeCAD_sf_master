/***************************************************************************
 *   Copyright (c) 2013 Luke Parry <l.parry@warwick.ac.uk>                 *
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

#include "FeatureViewOrthographic.h"

using namespace Drawing;

const char* FeatureViewOrthographic::TypeEnums[]= {"First Angle",
                                                   "Third Angle",
                                                   NULL};

PROPERTY_SOURCE(Drawing::FeatureViewOrthographic, Drawing::FeatureView)

FeatureViewOrthographic::FeatureViewOrthographic(void)
{
    static const char *group = "Drawing view";
    Type.setEnums(TypeEnums);
    ADD_PROPERTY_TYPE(Source    ,(0), group, App::Prop_None,"Shape to view");
    ADD_PROPERTY_TYPE(Views     ,(0), group, App::Prop_None,"Attached Views");
    ADD_PROPERTY(Type,((long)0));
}

short FeatureViewOrthographic::mustExecute() const
{
    if (Type.isTouched())
        return 1;
    return Drawing::FeatureView::mustExecute();
}

/// get called by the container when a Property was changed
void FeatureViewOrthographic::onChanged(const App::Property* prop)
{
    if (prop == &X ||
        prop == &Y ||
        prop == &Type ||
        prop == &Scale){
          if (!this->isRestoring()) {
              this->touch();
          }
    }
    Drawing::FeatureView::onChanged(prop);
}

FeatureViewOrthographic::~FeatureViewOrthographic()
{
}

void FeatureViewOrthographic::onDocumentRestored()
{
    this->execute();
}

App::DocumentObjectExecReturn *FeatureViewOrthographic::execute(void)
{
    // Rebuild the view
    const std::vector<App::DocumentObject *> &views = Views.getValues();
    for(std::vector<App::DocumentObject *>::const_iterator it = views.begin(); it != views.end(); ++it) {

        Drawing::FeatureView *view = dynamic_cast<Drawing::FeatureView *>(*it);

        // Set scale factor of each view
        view->Scale.setValue(this->Scale.getValue());
    }
    return Drawing::FeatureView::execute();
}

