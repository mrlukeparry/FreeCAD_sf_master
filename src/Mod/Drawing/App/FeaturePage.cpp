/***************************************************************************
 *   Copyright (c) J�rgen Riegel          (juergen.riegel@web.de) 2002     *
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

#include <Base/Exception.h>
#include <Base/Console.h>
#include <Base/FileInfo.h>
#include <App/Application.h>

#include <iostream>
#include <iterator>

#include "FeaturePage.h"
#include "FeatureView.h"
#include "FeatureViewCollection.h"
#include "FeatureClip.h"

using namespace Drawing;
using namespace std;


//===========================================================================
// FeaturePage
//===========================================================================

PROPERTY_SOURCE(Drawing::FeaturePage, App::DocumentObjectGroup)

const char *group = "Drawing view";

const char* FeaturePage::OrientationEnums[]= {"Portrait",
                                              "Landscape",
                                              NULL};

const char* FeaturePage::OrthoProjectionTypeEnums[]= {"First Angle",
                                                  "Third Angle",
                                                  NULL};

FeaturePage::FeaturePage(void)
{
    static const char *group = "Page";
    ADD_PROPERTY_TYPE(Views    ,(0), group, (App::PropertyType)(App::Prop_None),"Attached Views");
    ADD_PROPERTY_TYPE(Template ,(0), group, (App::PropertyType)(App::Prop_None),"Attached Template");

    // Projection Properties
    OrthoProjectionType.setEnums(OrthoProjectionTypeEnums);
    ADD_PROPERTY(OrthoProjectionType,((long)0));
    ADD_PROPERTY_TYPE(Scale ,(1.0)     ,group,App::Prop_None,"Scale factor for Document Views");

    // Physical Properties
    ADD_PROPERTY_TYPE(Width,(2)        ,group,(App::PropertyType)(App::Prop_None),"Width (mm)");
    ADD_PROPERTY_TYPE(Height,(7)       ,group,(App::PropertyType)(App::Prop_None),"Height(mm");
    ADD_PROPERTY_TYPE(PaperSize,("A3") ,group,(App::PropertyType)(App::Prop_None),"Paper Format");
    Orientation.setEnums(OrientationEnums);
    ADD_PROPERTY(Orientation,((long)0));
}

FeaturePage::~FeaturePage()
{
}


short FeaturePage::mustExecute() const
{
    // If Tolerance Property is touched
    if(Scale.isTouched())
        return 1;

    // Check the value of template if this has been modified
    App::DocumentObject* tmpl = Template.getValue();
    if(tmpl && tmpl->isTouched())
        return 1;

    // Check if within the selection, any Document Object have been touched
    bool ViewsTouched = false;
    const std::vector<App::DocumentObject*> &vals = Views.getValues();
    for(std::vector<App::DocumentObject *>::const_iterator it = vals.begin(); it < vals.end(); ++it) {
        if((*it)->isTouched()) {
            ViewsTouched = true;
        }
    }

    return (ViewsTouched) ? 1 : App::DocumentObjectGroup::mustExecute();
}

/// get called by the container when a Property was changed
void FeaturePage::onChanged(const App::Property* prop)
{
    App::DocumentObjectGroup::onChanged(prop);
}

int FeaturePage::addView(App::DocumentObject *docObj)
{
    if(!docObj->isDerivedFrom(Drawing::FeatureView::getClassTypeId()))
        return -1; //Doc Object must be derived from a Part Feature

    const std::vector<App::DocumentObject *> vals = Views.getValues();
    std::vector<App::DocumentObject *> newVals(vals);
    newVals.push_back(docObj);
    Views.setValues(newVals);

    if(docObj->getTypeId().isDerivedFrom(Drawing::FeatureViewCollection::getClassTypeId())) {
        // Add child views recursively to the page feature
        Drawing::FeatureViewCollection *collection = dynamic_cast<Drawing::FeatureViewCollection *>(docObj);
        const std::vector<App::DocumentObject *> & views = collection->Views.getValues();
        for(std::vector<App::DocumentObject*>::const_iterator it = views.begin(); it != views.end(); ++it) {
            this->addView(*it); // Recursively add child views
        }
    }

    Views.touch();
    return Views.getSize();
}

App::DocumentObjectExecReturn *FeaturePage::execute(void)
{
    Template.touch();
    Views.touch();
    return App::DocumentObject::StdReturn;
}