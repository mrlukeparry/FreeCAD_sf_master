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

#include <App/Document.h>
#include <App/DocumentObject.h>

#include <Base/Console.h>
#include <Base/Exception.h>

#include "FeatureOrthoView.h"
#include "FeatureViewOrthographic.h"

using namespace Drawing;

const char* FeatureViewOrthographic::TypeEnums[]= {"First Angle",
                                                   "Third Angle",
                                                   NULL};

PROPERTY_SOURCE(Drawing::FeatureViewOrthographic, Drawing::FeatureViewCollection)

FeatureViewOrthographic::FeatureViewOrthographic(void)
{
    static const char *group = "Drawing view";
    Type.setEnums(TypeEnums);
    ADD_PROPERTY(Type,((long)0));
}

short FeatureViewOrthographic::mustExecute() const
{
    if(Views.isTouched() ||
       Source.isTouched()) {
        return 1;
        Base::Console().Log("Oview touched");
     }

    if (Type.isTouched())
        return 1;
    return Drawing::FeatureViewCollection::mustExecute();
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
    Drawing::FeatureViewCollection::onChanged(prop);
}

FeatureViewOrthographic::~FeatureViewOrthographic()
{
}

bool FeatureViewOrthographic::hasView(const char *viewProjType)
{
    const std::vector<App::DocumentObject *> &views = Views.getValues();

    for(std::vector<App::DocumentObject *>::const_iterator it = views.begin(); it != views.end(); ++it) {

        Drawing::FeatureView *view = dynamic_cast<Drawing::FeatureView *>(*it);
        if(view->getClassTypeId() == Drawing::FeatureOrthoView::getClassTypeId()) {
            Drawing::FeatureOrthoView *orthoView = dynamic_cast<Drawing::FeatureOrthoView *>(*it);

            if(strcmp(viewProjType, orthoView->Type.getValueAsString()) == 0)
                return true;
        }
    }
    return false;
}

int FeatureViewOrthographic::addView(const char *viewProjType)
{
    // Find a more elegant way of validating the type
    if(strcmp(viewProjType, "Front")  == 0 ||
       strcmp(viewProjType, "Left")   == 0 ||
       strcmp(viewProjType, "Right")  == 0 ||
       strcmp(viewProjType, "Top")    == 0 ||
       strcmp(viewProjType, "Bottom") == 0 ||
       strcmp(viewProjType, "Rear")  == 0 ) {

        if(hasView(viewProjType)) {
            throw Base::Exception("The Projection is already used in this group");
        }

        std::string FeatName = this->getDocument()->getUniqueObjectName("OrthoView");
        App::DocumentObject *docObj = this->getDocument()->addObject("Drawing::FeatureOrthoView",
                                                                     FeatName.c_str());
        Drawing::FeatureOrthoView *view = dynamic_cast<Drawing::FeatureOrthoView *>(docObj);
        view->Source.setValue(this->Source.getValue());
        view->Scale.setValue(this->Scale.getValue());
        view->Type.setValue(viewProjType);

        std::string label = viewProjType;
        view->Label.setValue(label);

        // Add the new view to the collection
        std::vector<App::DocumentObject *> newViews(Views.getValues());
        newViews.push_back(docObj);
        Views.setValues(newViews);

        this->touch();
        return 1;
    } else if(strcmp(viewProjType, "Top Right")  == 0 ||
              strcmp(viewProjType, "Top Left")  == 0 ||
              strcmp(viewProjType, "Bottom Right")  == 0 ||
              strcmp(viewProjType, "Bottom Left")  == 0) {
        // Add an isometric view of the part
    }
    return -1;
}

void FeatureViewOrthographic::onDocumentRestored()
{
    this->execute();
}

App::DocumentObjectExecReturn *FeatureViewOrthographic::execute(void)
{
    return Drawing::FeatureViewCollection::execute();
}

