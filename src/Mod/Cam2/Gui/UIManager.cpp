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
#include "../App/CamManager.h"
#include "../App/TPGList.h"
#include "../App/TPGFeature.h"
#include "../App/TPG/PyTPGFactory.h"
#include "../App/TPG/TPG.h"

#include "UIManager.h"
#include "TPGListModel.h"
#include "ViewProviderCamFeature.h"

using namespace CamGui;

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

	// receive tpg running state changes from Cam layer.
	QObject::connect(&Cam::CamManager(), SIGNAL(updatedTPGState(QString, Cam::TPG::State, int)),
			this, SLOT(updatedTPGState(QString, Cam::TPG::State, int)));
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

    // Test settings editor
    Cam::TPG* tpg = tpgDescriptor->make();
    Q_EMIT updatedTPGSelection(tpg);

    Base::Console().Log("This is where I would add a '%s' TPG to the document", tpgDescriptor->name.toStdString().c_str());
  
//    // TPG was successfully created
//    // Find currently active CamFeature and add create a new TPGFeature and assign the TPG
//    Cam::CamFeature *camFeat = NULL;
//
//    Gui::Document * doc = Gui::Application::Instance->activeDocument();
//    if (doc->getInEdit() && doc->getInEdit()->isDerivedFrom(ViewProviderCamFeature::getClassTypeId())) {
//        ViewProviderCamFeature *vp = dynamic_cast<ViewProviderCamFeature*>(doc->getInEdit());
//        if(!vp) {
//            Base::Console().Log("An invalid view provider is currently being used");
//            return;
//        }
//        camFeat = vp->getObject();
//    }
//
//    // Create a new TPG Feature
//    if (camFeat != NULL) {
//        std::string tpgFeatName = doc->getDocument()->getUniqueObjectName("TPGFeature");
//        App::DocumentObject *tpgDocObj =  doc->getDocument()->addObject("Cam::TPGFeature", tpgFeatName.c_str());
//
//        if(!tpgDocObj || !tpgDocObj->isDerivedFrom(Cam::TPGFeature::getClassTypeId()))
//            return;
//
//        Cam::TPGFeature *tpgFeat = dynamic_cast<Cam::TPGFeature *>(tpgDocObj);
//
//        // Set a friendly label
//        if (tpgFeat != NULL) {
//            tpgFeat->Label.setValue(tpgDescriptor->name.toAscii());
//            Cam::TPG *temp = tpgDescriptor->make();
//            QMessageBox(QMessageBox::Warning, QString::fromAscii("Info"), temp->getName());
//            // Attempt to create and load the TPG Plugin
//            bool loadPlugin = tpgFeat->loadTPG(tpgDescriptor);
//
//            if(loadPlugin) {
//                // Add the Plugin to the TPG Feature's container
//                camFeat->getTPGContainer()->addTPG(tpgFeat);
//            } else {
//                QMessageBox(QMessageBox::Warning, QString::fromAscii("Info"), QString::fromAscii("Plugin couldn't be loaded"));
//                // remove TPGFeature
//                doc->getDocument()->remObject(tpgFeatName.c_str());
//            }
//        }
//        else
//            Base::Console().Log("Unable to find TPG Feature");
//    }
//    else
//        Base::Console().Log("Unable to find Cam Feature");
}
/**
 * A Slot to request a Library reload.
 */
void UIManagerInst::reloadTPGs()
{
    // scan for TPGs
    Cam::TPGFactory().scanPlugins();

    // get the TPGs
    Cam::TPGDescriptorCollection *plugins = Cam::TPGFactory().getDescriptors();

    CamGui::TPGListModel *model = new CamGui::TPGListModel(plugins);
    plugins->release();
    Q_EMIT updatedTPGList(model);
}

/**
 * A Slot to relay TPG state updates to UI Components
 */
void UIManagerInst::updatedTPGState(QString tpgid, Cam::TPG::State state, int progress) {

	Q_EMIT updatedTPGStateSig(tpgid, state, progress);
}

#include "moc_UIManager.cpp"
