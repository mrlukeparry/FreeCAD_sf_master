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
#ifndef _PreComp_
#include <QMenu>
#endif

#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Control.h>
#include <Gui/Document.h>
#include <Gui/SoFCSelection.h>
#include <Gui/Selection.h>
#include <Gui/SoTextLabel.h>
#include <Gui/MainWindow.h>
#include <Gui/SoFCUnifiedSelection.h>
#include <Gui/SoFCBoundingBox.h>
#include <Gui/View3DInventor.h>


#include "../App/CamFeature.h"
#include "../App/StockGeometry.h"
#include "../App/CamPartsList.h"
#include "../App/TPGList.h"
#include "../App/GCodeFeature.h"

#include "TaskDialog/TaskDlgEditCamFeature.h"
#include "ViewProviderCamFeature.h"

using namespace CamGui;

PROPERTY_SOURCE(CamGui::ViewProviderCamFeature, Gui::ViewProviderDocumentObject)

ViewProviderCamFeature::ViewProviderCamFeature()
{
}

ViewProviderCamFeature::~ViewProviderCamFeature()
{
}

void ViewProviderCamFeature::setupContextMenu(QMenu *menu, QObject *receiver, const char *member)
{
    menu->addAction(QObject::tr("Edit CamFeature"), receiver, member);
}

bool ViewProviderCamFeature::setEdit(int ModNum)
{
    /*When double-clicking on the item for this sketch the
    object unsets and sets its edit mode without closing
    the task panel */
    Gui::TaskView::TaskDialog *dlg = Gui::Control().activeDialog();
    TaskDlgEditCamFeature *taskDlgCamFeat = qobject_cast<TaskDlgEditCamFeature *>(dlg);

    if (dlg && !taskDlgCamFeat) {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("A dialog is already open in the task panel"));
        msgBox.setInformativeText(QObject::tr("Do you want to close this dialog?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes)
            Gui::Control().closeDialog();
        else
            return false;
    }
    Gui::Selection().clearSelection();


    //start the edit dialog
    if (taskDlgCamFeat)
        Gui::Control().showDialog(taskDlgCamFeat);
    else
        Gui::Control().showDialog(new TaskDlgEditCamFeature(this));

    return true;
}

void ViewProviderCamFeature::setEditViewer(Gui::View3DInventorViewer* viewer, int ModNum)
{
    viewer->setEditing(TRUE);
//     SoNode* root = viewer->getSceneGraph();
//     //static_cast<Gui::SoFCUnifiedSelection*>(root)->selectionRole.setValue(FALSE);
}

bool ViewProviderCamFeature::doubleClicked(void)
{
    Gui::Application::Instance->activeDocument()->setEdit(this);
    return true;
}

void ViewProviderCamFeature::unsetEditViewer(Gui::View3DInventorViewer* viewer)
{
    viewer->setEditing(FALSE);
}

void ViewProviderCamFeature::unsetEdit(int ModNum)
{
    // clear the selection and set the new/edited sketch(convenience)
    Gui::Selection().clearSelection();
}

std::vector<App::DocumentObject*> ViewProviderCamFeature::claimChildren(void) const
{
    // Collect any child fields and put this in the CamFeature tree
    std::vector<App::DocumentObject*> temp;
    Cam::CamFeature *feat = static_cast<Cam::CamFeature*>(getObject());
    try {
      temp.push_back(dynamic_cast<App::DocumentObject* >(feat->getStockGeometry()));
      temp.push_back(dynamic_cast<App::DocumentObject* >(feat->getPartsContainer()));
      temp.push_back(dynamic_cast<App::DocumentObject* >(feat->getTPGContainer()));
      temp.push_back(dynamic_cast<App::DocumentObject* >(feat->getGCodeFeature()));
      return temp;
    } catch (...) {
        std::vector<App::DocumentObject*> tmp;
        return tmp;
    }
}

Cam::CamFeature* ViewProviderCamFeature::getObject() const
{
    return dynamic_cast<Cam::CamFeature*>(pcObject);
}