/***************************************************************************
 *   Copyright (c) J�rgen Riegel          (juergen.riegel@web.de) 2007     *
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

#ifndef _DRAWING_FeaturePage_h_
#define _DRAWING_FeaturePage_h_

#include <App/DocumentObjectGroup.h>
#include <App/PropertyStandard.h>

namespace Drawing
{

/** Base class of all View Features in the drawing module
 */
class DrawingExport FeaturePage: public App::DocumentObjectGroup
{
    PROPERTY_HEADER(Drawing::FeaturePage);

public:
    FeaturePage(); /// Constructor
    ~FeaturePage();

    App::PropertyLinkList Views;
    App::PropertyLink Template;

    App::PropertyFloat Scale;
    App::PropertyEnumeration OrthoProjectionType; // First or Third Angle


    /** @name methods overide Feature */
    //@{
    /// recalculate the Feature
    virtual App::DocumentObjectExecReturn *execute(void);
    //@}
    //App::DocumentObjectExecReturn * recompute(void);

    int addView(App::DocumentObject *docObj);

    short mustExecute() const;

    /// returns the type name of the ViewProvider
    virtual const char* getViewProviderName(void) const {
        return "DrawingGui::ViewProviderDrawingPage";
    }
public:

    double getPageWidth() const;
    double getPageHeight() const;
    const char* getPageOrientation() const;

protected:
    void onChanged(const App::Property* prop);

private:
    static const char* OrthoProjectionTypeEnums[];
};


} //namespace Drawing

#endif //_DRAWING_FeaturePage_h_
