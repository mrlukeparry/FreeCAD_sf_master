/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <qobject.h>
#endif

#include "Workbench.h"
#include <Gui/MainWindow.h>
#include <Gui/MenuManager.h>
#include <Gui/ToolBarManager.h>
#include <Gui/DockWindowManager.h>

#include "ToolPathDockWindow.h"

using namespace CamGui;

/// @namespace CamGui @class Workbench
TYPESYSTEM_SOURCE(CamGui::Workbench, Gui::StdWorkbench)

Workbench::Workbench()
{
  Gui::DockWindowManager* pDockMgr = Gui::DockWindowManager::instance();
  Gui::MainWindow* pMainWindow = Gui::MainWindow::getInstance();

  // Add Toolpath Dock Window
  ToolPathDockWindow* pcTPView = new ToolPathDockWindow(0, pMainWindow);
  pcTPView->setObjectName("Tool Path");
  pcTPView->setMinimumWidth(150);
  pDockMgr->registerDockWindow("Cam_ToolPathDockWindow", pcTPView);
}

Workbench::~Workbench()
{
}

Gui::MenuItem* Workbench::setupMenuBar() const
{
    Gui::MenuItem* root = StdWorkbench::setupMenuBar();
    Gui::MenuItem* item = root->findItem("&Windows");

    Gui::MenuItem* part = new Gui::MenuItem();
    root->insertItem(item, part);
    part->setCommand("Cam");
    *part << "Cam_CreateCamFeature";

    return root;
}

Gui::ToolBarItem* Workbench::setupToolBars() const
{
    Gui::ToolBarItem* root = StdWorkbench::setupToolBars();

    Gui::ToolBarItem * part = new Gui::ToolBarItem(root);
    part->setCommand("Cam");
    *part << "Cam_CreateCamFeature";

    return root;
}

Gui::ToolBarItem* Workbench::setupCommandBars() const
{
    Gui::ToolBarItem* root = new Gui::ToolBarItem;
    return root;
}


Gui::DockWindowItems* Workbench::setupDockWindows() const
{
    Gui::DockWindowItems* root = Gui::StdWorkbench::setupDockWindows();
    root->addDockWidget("Cam_ToolPathDockWindow", Qt::BottomDockWidgetArea, true, false);
//    root->setVisibility(false); // hide all dock windows by default
//    root->setVisibility("Std_CombiView",true); // except of the combi view
//    root->setVisibility("Cam_ToolPathDockWindow",true); // Show ToolPathDockWindow
    return root;
}
