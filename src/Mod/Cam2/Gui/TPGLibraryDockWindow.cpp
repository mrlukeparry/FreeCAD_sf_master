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
# include <qobject.h>
# include <qmessagebox.h>
# include <Gui/MainWindow.h>
#endif
#include "ui_TPGLibraryDockWindow.h"
#include "TPGLibraryDockWindow.h"
#include "UIManager.h"

namespace CamGui {

TPGLibraryDockWindow::TPGLibraryDockWindow(Gui::Document*  pcDocument, QWidget *parent)
  : DockWindow(pcDocument,parent) , ui(new Ui_TPGLibraryDockWindow)
{
  setWindowTitle(tr("TPG Library"));

  ui->setupUi(this);

  updatedTPGList(NULL);

  /// Internal ///
  // connect add button to internal handler
  QObject::connect(ui->AddTPG, SIGNAL(clicked()),
      this, SLOT(addBtnClick()));

  /// External ///
  // connect addTPG to UIManager
  QObject::connect(this, SIGNAL(addTPG(Cam::TPGDescriptor*)),
      &UIManager(), SLOT(reloadTPGs()));
  // connect reload button to UIManager
  QObject::connect(ui->ReloadLibrary, SIGNAL(clicked()),
      &UIManager(), SLOT(reloadTPGs()));
  // subscribe to tpg list updates updatedTPGList
  QObject::connect(&UIManager(), SIGNAL(updatedTPGList(TPGListModel*)),
      this, SLOT(updatedTPGList(TPGListModel*)));
}

TPGLibraryDockWindow::~TPGLibraryDockWindow() {
}



/**
 * Set the model used to select TPGs from
 */
void TPGLibraryDockWindow::updatedTPGList(TPGListModel* tpgs)
{
  this->tpgs = tpgs;
  ui->TPGList->setModel(tpgs);
}


void TPGLibraryDockWindow::addBtnClick()
{
  QItemSelectionModel *selectionModel = ui->TPGList->selectionModel();
  QModelIndexList idxs = selectionModel->selectedIndexes();
  for (int i = 0; i < idxs.size(); i++) {
    Cam::TPGDescriptor *tpg = tpgs->get(i);
    if (tpg != NULL)
      Q_EMIT addTPG(tpg);
  }
}
//void TPGLibraryDockWindow::reloadBtnClick()
//{
////  QMessageBox::information(Gui::getMainWindow(), "Information", "This is where I would reload the TPG List");
////  Q_EMIT reloadLibraryRequest(ui->TPGList);
//}


#include "moc_TPGLibraryDockWindow.cpp"

} /* namespace CamGui */
