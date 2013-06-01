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

#include "../PreCompiled.h"
#ifndef _PreComp_
#endif

#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Control.h>
#include <Gui/Document.h>
#include <Gui/SelectionFilter.h>

#include "../../App/Features/StockGeometry.h"
#include "../ViewProviderStockGeometry.h"
#include "TaskDlgEditStockGeometry.h"

using namespace CamGui;


//**************************************************************************
//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDlgEditStockGeometry::TaskDlgEditStockGeometry(ViewProviderStockGeometry *vp)
    : TaskDialog(), stockGeomView(vp)
{
    assert(vp);
}

TaskDlgEditStockGeometry::~TaskDlgEditStockGeometry()
{

}

//==== calls from the TaskView ===============================================================

void TaskDlgEditStockGeometry::open()
{

}

void TaskDlgEditStockGeometry::clicked(int)
{
    
}

void TaskDlgEditStockGeometry::onSelectionChanged(const Gui::SelectionChanges& msg)
{

}

bool TaskDlgEditStockGeometry::accept()
{

    Gui::SelectionFilter selectionFilter("SELECT Part::Feature COUNT 1");

    if (selectionFilter.match()) {
        stockGeomView->getObject()->Geometry.setValue(selectionFilter.Result[0][0].getObject());
    } else {
        return false;
    }

    std::string document = getDocumentName(); // needed because resetEdit() deletes this instance
    Gui::Command::doCommand(Gui::Command::Gui,"Gui.getDocument('%s').resetEdit()", document.c_str());
    Gui::Command::doCommand(Gui::Command::Doc,"App.getDocument('%s').recompute()", document.c_str());
        
}

bool TaskDlgEditStockGeometry::reject()
{
    std::string document = getDocumentName(); // needed because resetEdit() deletes this instance
    Gui::Command::doCommand(Gui::Command::Gui,"Gui.getDocument('%s').resetEdit()", document.c_str());
    Gui::Command::doCommand(Gui::Command::Doc,"App.getDocument('%s').recompute()", document.c_str());
    return true;
}

void TaskDlgEditStockGeometry::helpRequested()
{

}

#include "TaskDialog/moc_TaskDlgEditStockGeometry.cpp"
