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

#ifndef CAM_STOCKGEOMETRY_H
#define CAM_STOCKGEOMETRY_H

#include <App/PropertyStandard.h>
#include <App/DocumentObject.h>
#include "Feature.h"

namespace Cam
{
/*
 *  Stock Geometry provides a base link to a 3D Part geometry within the document for
 * for defining the input geometry. It was also provide material properties that can be
 * used by tools */
class CamExport StockGeometry : public App::DocumentObject
{
    PROPERTY_HEADER(Cam::StockGeometry);

public:
    StockGeometry();
    ~StockGeometry();

    /// App Properties
    App::PropertyLink Geometry; // Reference link to the Stock Material

    short mustExecute() const;

    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute(void);

    const char* getViewProviderName(void) const {
        return "CamGui::ViewProviderStockGeometry";
    }
    /// updates the Placement property from the Placement of Base
//     void positionByBase(void);

protected:
    void onChanged(const App::Property* prop);
};

} //namespace Cam


#endif // CAM_STOCKGEOMETRY_H
