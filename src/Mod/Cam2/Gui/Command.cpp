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

#include "../App/CamFeature.h"
#include "../App/StockGeometry.h"
#include "../App/TPGList.h"
#include "../App/TPGFeature.h"
#include "../App/CamPartsList.h"


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
    sPixmap       = "cam";
}

void CmdCamCreateCamFeature::activated(int iMsg)
{

    std::string FeatName = getUniqueObjectName("CamFeature");
    App::Document *doc   = getActiveGuiDocument()->getDocument();

    // NOTE Need to use simple test case file
    App::DocumentObject *camFeat =  doc->addObject("Cam::CamFeature", FeatName.c_str());

    // Initialise a few TPG Features and put this in tree for testing

    App::DocumentObject *docObj = doc->getObject(FeatName.c_str());

    if(docObj && docObj->isDerivedFrom(Cam::CamFeature::getClassTypeId())) {
        Cam::CamFeature *camFeat = dynamic_cast<Cam::CamFeature *>(docObj);

        // We Must Initialise the Cam Feature before usage
        camFeat->initialise();        
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
