/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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
#include <Base/Console.h>
#include <Base/Exception.h>

#include "FeatureView.h"

using namespace Drawing;

//===========================================================================
// FeatureView
//===========================================================================

PROPERTY_SOURCE(Drawing::FeatureView, App::DocumentObject)

FeatureView::FeatureView(void)
{
    static const char *group = "Drawing view";
    ADD_PROPERTY_TYPE(X ,(0)       ,group,App::Prop_None,"X position of the view on the drawing in modelling units (mm)");
    ADD_PROPERTY_TYPE(Y ,(0)       ,group,App::Prop_None,"Y position of the view on the drawing in modelling units (mm)");
    ADD_PROPERTY_TYPE(Scale ,(1.0) ,group,App::Prop_None,"Scale factor of the view");
    ADD_PROPERTY_TYPE(Rotation ,(0),group,App::Prop_None,"Rotation of the view in degrees counterclockwise");


    App::PropertyType type = (App::PropertyType)(App::Prop_Hidden);
}

/// get called by the container when a Property was changed
void FeatureView::onChanged(const App::Property* prop)
{
    if (prop == &X ||
        prop == &Y ||
        prop == &Scale ||
        prop == &Rotation) {
          if (!this->isRestoring()) {
              this->touch();
          }
    }
    App::DocumentObject::onChanged(prop);
}

FeatureView::~FeatureView()
{
}

void FeatureView::onDocumentRestored()
{
    // Rebuild the view
    this->execute();
}

App::DocumentObjectExecReturn *FeatureView::execute(void)
{
    return App::DocumentObject::StdReturn;
}

// Python Drawing feature ---------------------------------------------------------

namespace App {
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(Drawing::FeatureViewPython, Drawing::FeatureView)
template<> const char* Drawing::FeatureViewPython::getViewProviderName(void) const {
    return "DrawingGui::ViewProviderDrawingView";
}
/// @endcond

// explicit template instantiation
template class DrawingExport FeaturePythonT<Drawing::FeatureView>;
}
