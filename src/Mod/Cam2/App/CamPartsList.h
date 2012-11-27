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

#ifndef CAM_CAMPARTS_H
#define CAM_CAMPARTS_H

#include <App/DocumentObject.h>
#include <App/PropertyStandard.h>
#include <App/FeaturePython.h>

#include <Mod/Part/App/PartFeature.h>

class TopoDS_Shape;
class TopoDS_Face;

namespace Cam
{

  /**
 * This class needs to manage the list of INPUT geometry that is used by each TPG
 * and provide methods of extracting geometry shapes from the TPG that are independant of OpenCASCADE
 **/  
class CamExport CamPartsList : public App::DocumentObject
{
    PROPERTY_HEADER(Cam::CamPartsList);

public:
    CamPartsList();
    ~CamPartsList();

    short mustExecute() const;

    /// Property
    App::PropertyFloat       Tolerance;
    App::PropertyLinkList    ExternalGeometry;

    /** @name methods overide Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute(void);

    /// returns the type name of the ViewProvider
    const char* getViewProviderName(void) const {
        return "CamGui::ViewProviderCamPartsList";
    }
    //@}

    /// Manage the list of geometry
    int addPart(App::DocumentObject *docObj);
    int addPart(const char *);
    int removePart(const char *);
    int clearParts();

    CamPartsList * getPartsList();
    void setTolerance(float val) { Tolerance.setValue(val); }

    // Method for processing geometry for 3D surfaces

    /// from base class
//     virtual PyObject *getPyObject(void);
    virtual unsigned int getMemSize(void) const;
    virtual void Save(Base::Writer &/*writer*/) const;
    virtual void Restore(Base::XMLReader &/*reader*/);

protected:

      /// Useful helpful function for calculating the mesh for individual face
    void generateGeometry();
    void transferToArray(const TopoDS_Face& aFace,gp_Vec** vertices,gp_Vec** vertexnormals, long** cons,int &nbNodesInFace,int &nbTriInFace );

    /// get called by the container when a property has changed
    virtual void onChanged(const App::Property* /*prop*/);
    virtual void onDocumentRestored();
    virtual void onFinishDuplicating();

private:

};

// typedef App::FeaturePythonT<CamFeature> CamFeaturetPython;

} //namespace Cam

#endif // CAM_CAMPARTS_H