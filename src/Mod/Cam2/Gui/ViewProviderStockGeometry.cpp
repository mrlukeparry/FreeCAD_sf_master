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

#include "TaskDialog/TaskDlgEditCamFeature.h"
#include "TaskDialog/TaskDlgEditStockGeometry.h"
#include "../App/Features/StockGeometry.h"
#include "ViewProviderStockGeometry.h"

using namespace CamGui;

PROPERTY_SOURCE(CamGui::ViewProviderStockGeometry, Gui::ViewProviderDocumentObject)

ViewProviderStockGeometry::ViewProviderStockGeometry()
{
}

ViewProviderStockGeometry::~ViewProviderStockGeometry()
{
}

void ViewProviderStockGeometry::setupContextMenu(QMenu *menu, QObject *receiver, const char *member)
{
    menu->addAction(QObject::tr("Edit Stock Geometry"), receiver, member);
}


bool ViewProviderStockGeometry::setEdit(int ModNum)
{
   // When double-clicking on the item for this sketch the
    // object unsets and sets its edit mode without closing
    // the task panel
    Gui::TaskView::TaskDialog *dlg = Gui::Control().activeDialog();
    TaskDlgEditStockGeometry *taskDlgStockGeom = qobject_cast<TaskDlgEditStockGeometry *>(dlg);
    
//     if (sketchDlg && sketchDlg->getSketchView() != this)
//         sketchDlg = 0; // another sketch left open its task panel
    if (dlg && !taskDlgStockGeom ) {
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

    // Set the selection to what was stored
    if(getObject()->Geometry.getValue() != NULL)
    {
        std::vector<App::DocumentObject *> objs;
        objs.push_back(getObject()->Geometry.getValue());
        Gui::Selection().setSelection(getObject()->getDocument()->getName(), objs);
    }
    
    //start the edit dialog
    if (taskDlgStockGeom)
        Gui::Control().showDialog(taskDlgStockGeom);
    else
        Gui::Control().showDialog(new TaskDlgEditStockGeometry(this));

    return true;
}

void ViewProviderStockGeometry::setEditViewer(Gui::View3DInventorViewer* viewer, int ModNum)
{
    viewer->setEditing(TRUE);
//     SoNode* root = viewer->getSceneGraph();
//     //static_cast<Gui::SoFCUnifiedSelection*>(root)->selectionRole.setValue(FALSE);
}

bool ViewProviderStockGeometry::doubleClicked(void)
{
    Gui::Application::Instance->activeDocument()->setEdit(this);
    return true;
}

void ViewProviderStockGeometry::unsetEditViewer(Gui::View3DInventorViewer* viewer)
{
    viewer->setEditing(FALSE);
}

void ViewProviderStockGeometry::unsetEdit(int ModNum)
{
    // clear the selection and set the new/edited sketch(convenience)
  // clear the selection and set the new/edited sketch(convenience)
    Gui::Selection().clearSelection();

    // when pressing ESC make sure to close the dialog
    Gui::Control().closeDialog();
}

Cam::StockGeometry* ViewProviderStockGeometry::getObject() const
{
    return dynamic_cast<Cam::StockGeometry*>(pcObject);
}
