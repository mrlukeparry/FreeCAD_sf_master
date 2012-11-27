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

#ifndef CAM_TPGFEATURE_H
#define CAM_TPGFEATURE_H

#include <App/DocumentObject.h>
#include <App/PropertyLinks.h>
#include <App/PropertyStandard.h>
#include <Mod/Part/App/PartFeature.h>
#include <Base/BoundBox.h>

#include "TPG/TPGFactory.h"
#include "TPG/TPG.h"

/**
  * TPGFeature is the document object that provides the user interface to control an individual ToolPathGenerator
  * It is designed to be used both standalone or in a TPGList
  */
namespace Cam
{
class CamExport TPGFeature : public App::DocumentObject
{
    PROPERTY_HEADER(Cam::TPGFeature);

public:
    TPGFeature();
    TPGFeature(TPGDescriptor *tpgDescriptor);
    ~TPGFeature();

    ///App properties stores (saves and restores references used by the TPG)
    App::PropertyLinkSubList   ExternalGeometry;

    /// Methods for creating external interface to attach input to each TPG
    // [TODO] eventually this could be an APP::Property link list but doesn't make sens
    void setInputGeometry(const std::vector<Part::Feature *> & vals) { inputGeometry = std::vector<Part::Feature *>(vals); } //Unsure whether we should convert this in here;
    void setBoundingBox(const Base::BoundBox3d & bbox) { inputBBox = bbox; }

    /// Methods for obtaining output the output
    /*virtual getGCode(); //TODO this*/
    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute(void);

    const char* getViewProviderName(void) const {
        return "CamGui::ViewProviderTPGFeature";
    }

    void initialise();

    /// Methods for controlling the TPG
    void run();
    void stop();

    /// Methods for handling the TPG
    bool loadTPG(TPGDescriptor *tpgDescriptor);
    TPG * getTPG() const { return (this->hasTPG()) ? tpg : NULL ; }

    /// Convenience method for get the current TPG Status (Undefined if TPG not loaded)
    TPG::State getTPGStatus();
    bool hasTPG() const { return (tpg != NULL); }
    bool hasRunningTPG();

protected:
    TPG *tpg;
    Base::BoundBox3d inputBBox;
    std::vector<Part::Feature *> inputGeometry;
};

} //namespace Cam


#endif //CAM_TPGFEATURE_H
