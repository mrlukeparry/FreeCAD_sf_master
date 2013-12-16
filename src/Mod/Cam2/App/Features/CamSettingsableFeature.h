/***************************************************************************
 *   Copyright (c) 2012 Luke Parry    (l.parry@warwick.ac.uk)              *
 *   Copyright (c) 2013 Andrew Robinson <andrewjrobinson@gmail.com>        *
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

#ifndef CAM_SETTINGSABLEFEATURE_H
#define CAM_SETTINGSABLEFEATURE_H

//#define CamExport
#include <PreCompiled.h>

namespace Cam {
	namespace Settings {
		class CamExport Feature;
	}
}

#include <boost/signals.hpp>

#include <App/DocumentObject.h>
#include <App/PropertyLinks.h>
#include <App/PropertyStandard.h>
#include <Base/BoundBox.h>
#include <Mod/Part/App/PartFeature.h>

typedef boost::signals::connection Connection;

/**
  * CamSettingsableFeature is a document object super-type for all Cam document objects
  * that make use the the advanced settings dock-window.
  */
namespace Cam
{
	namespace Settings
	{
class CamExport Feature : public App::DocumentObject
{
	PROPERTY_HEADER(Cam::Settings::Feature);

public:
    Feature();
    ~Feature();

	App::PropertyMap           Values;

	bool IsCamSettingsProperty(const App::Property* prop) const;

	void setValue(const std::string & key, const std::string & value);

	const std::map<std::string,std::string> &getValues(void) const;

    /// recalculate the Feature
    App::DocumentObjectExecReturn *execute(void);

    const char* getViewProviderName(void) const {
        return "CamGui::ViewProviderCamSettingsableFeature";
    }

	void initialise();

//    void onDelete(const App::DocumentObject &docObj);

//    virtual void Save(Base::Writer &/*writer*/) const;
//    virtual void Restore(Base::XMLReader &/*reader*/);

	virtual void onBeforeChange(const App::Property* prop);
	virtual void onChanged(const App::Property* prop);

protected:
    
    ///Connections
    Connection delObjConnection;

    /// get called by the container when a property has changed
    //     virtual void onChanged(const App::Property* /*prop*/);
//    virtual void onSettingDocument();

//    virtual void onDocumentRestored();
};

} //namespace Settings
} //namespace Cam


#endif //CAM_SETTINGSABLEFEATURE_H
