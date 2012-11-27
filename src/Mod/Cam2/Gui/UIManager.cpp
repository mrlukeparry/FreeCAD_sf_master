/***************************************************************************
 *   Copyright (c) 2012 Andrew Robinson <andrewjrobinson@gmail.com>        *
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
#include <Python.h>
#endif

#include <QMessageBox>

#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>

#include <Base/Console.h>

#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/DockWindowManager.h>
#include <Gui/MainWindow.h>

#include "../App/CamFeature.h"
#include "../App/TPGList.h"
#include "../App/TPGFeature.h"
#include "../App/TPG/PyTPGManager.h"

#include "UIManager.h"
#include "TPGListModel.h"
#include "ViewProviderCamFeature.h"

namespace CamGui {

UIManagerInst* UIManagerInst::_pcSingleton = NULL;

UIManagerInst& UIManagerInst::instance()
{
    if (_pcSingleton == NULL)
        _pcSingleton = new UIManagerInst();

    return *_pcSingleton;
}
void UIManagerInst::destruct (void)
{
    if (_pcSingleton != NULL)
        delete _pcSingleton;
    _pcSingleton = NULL;
}

UIManagerInst::UIManagerInst() {
}

UIManagerInst::~UIManagerInst() {
}

/**
 * A Slot to receive requests to add TPG's to the document tree.
 */
void UIManagerInst::addTPG(Cam::TPGDescriptor *tpgDescriptor) 
{
    if (tpgDescriptor == NULL) {
        Base::Console().Error("This is an invalid plugin description");
        return;
    }

    Base::Console().Log("This is where I would add a '%s' TPG to the document", tpgDescriptor->name.toStdString().c_str());
  
    // TPG was successfully created
    // Find currently active CamFeature and add create a new TPGFeature and assign the TPG
    Cam::CamFeature *camFeat = NULL;

    Gui::Document * doc = Gui::Application::Instance->activeDocument();
    if (doc->getInEdit() && doc->getInEdit()->isDerivedFrom(ViewProviderCamFeature::getClassTypeId())) {
        ViewProviderCamFeature *vp = dynamic_cast<ViewProviderCamFeature*>(doc->getInEdit());
        if(!vp) {
            Base::Console().Log("An invalid view provider is currently being used");
            return;
        }
        camFeat = vp->getObject();
    }
    
    // Create a new TPG Feature
    std::string tpgFeatName = doc->getDocument()->getUniqueObjectName("TPGFeature");
    App::DocumentObject *tpgDocObj =  doc->getDocument()->addObject("Cam::TPGFeature", tpgFeatName.c_str());
    
    if(!tpgDocObj || !tpgDocObj->isDerivedFrom(Cam::TPGFeature::getClassTypeId()))
        return;
    
    Cam::TPGFeature *tpgFeat = dynamic_cast<Cam::TPGFeature *>(tpgDocObj);

    // Set a friendly label
    tpgFeat->Label.setValue(tpgDescriptor->name.toAscii());
    Cam::TPG *temp = tpgDescriptor->make();
    QMessageBox(QMessageBox::Warning, QString::fromAscii("Info"), temp->getName());
    // Attempt to create and load the TPG Plugin
    bool loadPlugin = tpgFeat->loadTPG(tpgDescriptor);
    
    if(loadPlugin) {
        // Add the Plugin to the TPG Feature's container
        camFeat->getTPGContainer()->addTPG(tpgFeat);
    } else {
        QMessageBox(QMessageBox::Warning, QString::fromAscii("Info"), QString::fromAscii("Plugin couldn't be loaded"));
        // remove TPGFeature
        doc->getDocument()->remObject(tpgFeatName.c_str());
    }

}
/**
 * A Slot to request a Library reload.
 */
void UIManagerInst::reloadTPGs()
{
    // scan for TPGs
    Cam::TPGFactory().scanPlugins();

    // get the TPGs
    std::vector<Cam::TPGDescriptor*> *plugins = Cam::TPGFactory().getDescriptors();

    CamGui::TPGListModel *model = new CamGui::TPGListModel(plugins);
    Q_EMIT updatedTPGList(model);
}

#include "moc_UIManager.cpp"
