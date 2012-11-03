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

#ifndef CAM_CAMFEATURE_H
#define CAM_CAMFEATURE_H

#include <App/DocumentObject.h>
#include <App/PropertyLinks.h>
#include <App/PropertyStandard.h>
#include <App/PropertyFile.h>
#include <App/FeaturePython.h>

#include <boost/signals.hpp>

typedef boost::signals::connection Connection;

namespace Cam
{

class CamPartsList;
class TPGList;
class StockGeometry;
class GCodeFeature;

class CamExport CamFeature : public App::DocumentObject
{
    PROPERTY_HEADER(Cam::CamFeature);

public:
    CamFeature(); ///Constructor
    virtual ~CamFeature(); ///Destructor

    /// This is only called on the creation of a new document object (must be made to intialise hierarchy
    void initialise();

    ///SLOTS
    void onDelete(const App::DocumentObject &docObj);

    /// Property
    App::PropertyLink     Result;      // Link to the GCodeFeature (End Result - TOP of TPG hierarchy)

    /** @name methods overide Base Class */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute(void);
    short mustExecute() const; // Check if recalculation is needed

    /// returns the type name of the ViewProvider
    const char* getViewProviderName(void) const {
        return "CamGui::ViewProviderCamFeature";
    }

    // from base class
    // virtual PyObject *getPyObject(void);
    virtual unsigned int getMemSize(void) const;
    virtual void Save(Base::Writer &/*writer*/) const;
    virtual void Restore(Base::XMLReader &/*reader*/);
    //@}

    /// Getters
    StockGeometry * getStockGeometry() const { return stockGeometry; }
    CamPartsList * getPartsContainer() const { return camPartsList; }
    TPGList * getTPGContainer() const { return tpgList; }
    GCodeFeature * getGCodeFeature() const { return getGCodeResult(); }

    GCodeFeature * getGCodeResult() const;

    void updateTPG();
    // Convenience method (Containment not necessarily needed)
    bool hasContainment() { return (stockGeometry == 0) ? true : false; }

protected:
    ///Connections
    Connection delObjConnection;

protected:
    /// get called by the container when a property has changed
    //virtual void onChanged(const App::Property* /*prop*/);
    virtual void onSettingDocument();
    virtual void onDocumentRestored();
    virtual void onFinishDuplicating();

    void rebuildLinks();

    CamPartsList    *camPartsList;
    TPGList         *tpgList;
    StockGeometry   *stockGeometry;
private:

};

// typedef App::FeaturePythonT<CamFeature> CamFeaturetPython;

} //namespace Cam

#endif // CAM_CAMFEATURE_H