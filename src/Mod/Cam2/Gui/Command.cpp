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

#include "PreCompiled.h"

#include <App/DocumentObjectGroup.h>
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/Application.h>
#include <Gui/MainWindow.h>
#include <QPointer>

#include <Mod/Cam2/App/CamFeature.h>
#include <Mod/Cam2/App/StockGeometry.h>
#include <Mod/Cam2/App/TPGList.h>
#include <Mod/Cam2/App/TPGFeature.h>
#include <Mod/Cam2/App/CamPartsList.h>


//===========================================================================
// CmdCamCreateCamFeature
//===========================================================================
DEF_STD_CMD_A(CmdCamCreateCamFeature);

CmdCamCreateCamFeature::CmdCamCreateCamFeature()
  :Command("Cam_CreateCamFeature")
{
    sAppModule    = "Cam";
    sGroup        = QT_TR_NOOP("Cam");
    sMenuText     = QT_TR_NOOP("Create a new Cam Feature");
    sToolTipText  = QT_TR_NOOP("Create a new Cam Feature");
    sWhatsThis    = sToolTipText;
    sStatusTip    = sToolTipText;
    sPixmap       = "Cam_NewCamFeature";
}

void CmdCamCreateCamFeature::activated(int iMsg)
{

    std::string FeatName = getUniqueObjectName("CamFeature");
//     openCommand("Raytracing create render feature");
//     doCommand(Doc,"import Raytracing,RaytracingGui");
//     doCommand(Doc,"App.activeDocument().addObject('Raytracing::RenderFeature','%s')",FeatName.c_str());
//
//
//     // Attach this temporary camera to the Render Feature
//     doCommand(Doc,"App.ActiveDocument.%s.attachRenderCamera(renderCam)", FeatName.c_str());
//
//     doCommand(Doc,"App.ActiveDocument.%s.setRenderPreset('metropolisUnbiased')", FeatName.c_str());
//     doCommand(Doc,"CamPartsLisApp.ActiveDocument.%s.setRenderTemplate('lux_default')", FeatName.c_str());
//
//     doCommand(Gui,"Gui.activeDocument().setEdit('%s')",FeatName.c_str());
//     commitCommand();

    App::Document *doc = getActiveGuiDocument()->getDocument();

    // NOTE Need to use simple test case file
    App::DocumentObject *stock = getActiveGuiDocument()->getDocument()->getObject("Pad");
    App::DocumentObject *pad = getActiveGuiDocument()->getDocument()->getObject("Pad001");

    doc->addObject("Cam::CamFeature", FeatName.c_str());

    // TODO Unsure if these should be created automatically by the CAM Feature in the constructor
    // -- maybe useful to keep it like this so we can create this in a wizard step by  step

    // Initialise a few TPG Features and put this in tree for testing

    std::string TPGFeatName1 = getUniqueObjectName("TPGFeature");
    doc->addObject("Cam::TPGFeature", TPGFeatName1.c_str());
    std::string TPGFeatName2 = getUniqueObjectName("TPGFeature");
    doc->addObject("Cam::TPGFeature", TPGFeatName2.c_str());
    std::string TPGFeatName3 = getUniqueObjectName("TPGFeature");
    doc->addObject("Cam::TPGFeature", TPGFeatName3.c_str());

    App::DocumentObject *docObj = doc->getObject(FeatName.c_str());

    if(docObj && docObj->isDerivedFrom(Cam::CamFeature::getClassTypeId())) {
        Cam::CamFeature *camFeat = dynamic_cast<Cam::CamFeature *>(docObj);

        // We Must Initialise the Cam Feature before usage
        camFeat->initialise();
        Cam::TPGList *tpgList = camFeat->getTPGContainer();
        tpgList->addTPG(TPGFeatName1.c_str());
        tpgList->addTPG(TPGFeatName2.c_str());
        tpgList->addTPG(TPGFeatName3.c_str());

        //Test Access
        const std::vector<App::DocumentObject *> tpgFeats = tpgList->TPGObjects.getValues();
        App::DocumentObject *tpgDocObj = tpgFeats.at(0);
                App::DocumentObject *tpgDocObj1 = tpgFeats.at(1);

        Cam::TPGFeature *tpgFeat = dynamic_cast<Cam::TPGFeature *>(tpgDocObj);
        Cam::TPGFeature *tpgFeat1 = dynamic_cast<Cam::TPGFeature *>(tpgDocObj1);

        tpgFeat->loadTPG("MyPlugin");
        camFeat->getPartsContainer()->addPart(pad);
        camFeat->getStockGeometry()->Geometry.setValue(stock);
    }

}

bool CmdCamCreateCamFeature::isActive(void)
{
    return hasActiveDocument();
}

void CreateCamCommands()
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
    rcCmdMgr.addCommand(new CmdCamCreateCamFeature());
}
