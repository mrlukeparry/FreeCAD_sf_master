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

#ifndef CAM_GCODEFEATURE_H
#define CAM_GCODEFEATURE_H

#include <QString>

#include <App/PropertyFile.h>
#include <App/PropertyLinks.h>
#include <App/PropertyStandard.h>
#include <App/DocumentObject.h>

/// Base class
// Holds and Compiles the GCode Output and allows interaction with the user.
namespace Cam
{
class TPGList;
class GCode;

class CamExport GCodeFeature : public App::DocumentObject
{
    PROPERTY_HEADER(Cam::GCodeFeature);

public:
    App::PropertyLink     TPGListObj;
    //App::PropertyLink     PostProcessor;

    /// Constructor & Destructor
    GCodeFeature();
    ~GCodeFeature();

    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute(void);
        /// returns the type name of the ViewProvider

    const char* getViewProviderName(void) const {
        return "CamGui::ViewProviderGCodeFeature";
    }

    TPGList * getTPGList() const;

    QString getResult() { return QString::fromAscii("empty"); }

protected:
    GCode *result;
};

} //namespace Cam


#endif //CAM_GCODEFEATURE_H
