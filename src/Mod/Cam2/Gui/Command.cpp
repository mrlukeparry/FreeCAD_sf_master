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

#include "PreCompiled.h"

#include <App/DocumentObjectGroup.h>
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/Application.h>
#include <Gui/MainWindow.h>
#include <QPointer>

#include "../App/Features/CamFeature.h"
#include "../App/Features/StockGeometry.h"
#include "../App/Features/TPGList.h"
#include "../App/Features/TPGFeature.h"
#include "../App/Features/CamPartsList.h"

#include "UIManager.h"


//===========================================================================
// CmdCamFeature
//===========================================================================
DEF_STD_CMD_A(CmdCamFeature);

CmdCamFeature::CmdCamFeature()
  :Command("Cam_CamFeature")
{
    sAppModule    = "Cam";
    sGroup        = QT_TR_NOOP("Cam");
    sMenuText     = QT_TR_NOOP("Create a new Cam Feature");
    sToolTipText  = QT_TR_NOOP("Create a new Cam Feature");
    sWhatsThis    = sToolTipText;
    sStatusTip    = sToolTipText;
    sPixmap       = "Cam_CamFeature";
}

void CmdCamFeature::activated(int iMsg)
{
    openCommand("CamFeatureNew");
	if (CamGui::UIManager().CamFeature()) {
//		updateActive();
    	commitCommand();
	}
	else
		abortCommand();
}

bool CmdCamFeature::isActive(void)
{
    return hasActiveDocument();
}

//===========================================================================
// CmdCamTPGFeature
//===========================================================================
DEF_STD_CMD_A(CmdCamTPGFeature);

CmdCamTPGFeature::CmdCamTPGFeature()
  :Command("Cam_TPGFeature")
{
    sAppModule    = "Cam";
    sGroup        = QT_TR_NOOP("Cam");
    sMenuText     = QT_TR_NOOP("Create a new TPG Feature");
    sToolTipText  = QT_TR_NOOP("Create a new TPG Feature");
    sWhatsThis    = sToolTipText;
    sStatusTip    = sToolTipText;
    sPixmap       = "Cam_TPGFeature";
}

void CmdCamTPGFeature::activated(int iMsg)
{
    openCommand("New TPG Feature");
	if (CamGui::UIManager().TPGFeature()) {
    	commitCommand();
	}
	else
		abortCommand();
}

bool CmdCamTPGFeature::isActive(void)
{
    return hasActiveDocument();
}

//===========================================================================
// Common Code
//===========================================================================
/**
 *
 */
void CreateCamCommands()
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
    rcCmdMgr.addCommand(new CmdCamFeature());
    rcCmdMgr.addCommand(new CmdCamTPGFeature());
}
