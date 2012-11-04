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

#ifndef CAM_TPGLIST_H
#define CAM_TPGLIST_H

#include <App/DocumentObject.h>
#include <App/PropertyStandard.h>
#include <App/PropertyFile.h>
#include <App/FeaturePython.h>

#include <Mod/Part/App/PartFeature.h>

namespace Cam
{

class StockGeometry;
class CamPartsList;
class TPGFeature;

/**
 * TPGList provides an intelligent collection to manage a series of operations created by TPGFeature and update these when necessary
 * It manages the TPGFeatures which behave essentially like blackboxes - they need input and create output but don't know anything
 * about their environment.
 * The TPGList must be used within a CAMFeature.
 */
class CamExport TPGList : public App::DocumentObject
{
    PROPERTY_HEADER(Cam::TPGList);

public:
    TPGList();
    ~TPGList();

    /// Property
    App::PropertyLinkList    TPGObjects;
    App::PropertyLink        StockGeometryObject;
    App::PropertyLink        CamPartsListObject;

    /** @name methods overide Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute(void);
    short mustExecute(void) const;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName(void) const {
        return "CamGui::ViewProviderTPGList";
    }
    //@}

    /// Getters
    CamPartsList * getCamPartsList() const;
    StockGeometry * getStockGeometry() const;
    std::vector<TPGFeature *> getTPGFeatureList();

    /// Manage the list of TPG Features
    int addTPG(App::DocumentObject *docObj);
    int addTPG(const char *name);
    int removeTPG(const char *name);

    /// Clear the TPG List of all TPGFeatures
    void clear();
    void updateAll();

    // Method for processing geometry for 3D surfaces

    /// from base class
    //     virtual PyObject *getPyObject(void);
    virtual unsigned int getMemSize(void) const;
    virtual void Save(Base::Writer &/*writer*/) const;
    virtual void Restore(Base::XMLReader &/*reader*/);

protected:

      /// Useful helpful function for calculating the mesh for individual face
    /// get called by the container when a property has changed
    virtual void onChanged(const App::Property* /*prop*/);
    virtual void onDocumentRestored();
    virtual void onFinishDuplicating();

private:

};

// typedef App::FeaturePythonT<CamFeature> CamFeaturetPython;

} //namespace Cam

#endif // CAM_TPGLIST_H