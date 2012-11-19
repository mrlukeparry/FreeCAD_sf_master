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

#define EMBED_BREAKPOINT  asm volatile ("int3;")

#include "PreCompiled.h"
#ifndef _PreComp_
//#include <qobject.h>
#include <Python.h>
#endif

#include <qmessagebox.h>

#include <Gui/DockWindowManager.h>
#include <Gui/MainWindow.h>

#include "UIManager.h"
#include "TPGListModel.h"

#include "../App/TPG/PyTPGManager.h"

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
void UIManagerInst::addTPG(Cam::TPGDescriptor *tpg) {
    if (tpg != NULL)
        QMessageBox::information(Gui::getMainWindow(), "Information",
                "This is where I would add a '" + tpg->name
                        + "' TPG to the document");
    Cam::TPG *tp = tpg->make();
    delete tp;
}
/**
 * A Slot to request a Library reload.
 */
void UIManagerInst::reloadTPGs()
{
  if (!Py_IsInitialized())
    QMessageBox::information( Gui::getMainWindow(), "Information", "Python not initialised" );
  else {
    // get the TPGs
    std::vector<Cam::TPGDescriptor*> *plugins = &Cam::PyTPGManager().scanPlugins();
    printf("Plugins:\n");
    for (int i = 0; i < plugins->size(); i++) {
      (*plugins)[i]->print();
    }

    CamGui::TPGListModel *model = new CamGui::TPGListModel(plugins);
    Q_EMIT updatedTPGList(model);
  }
}

#include "moc_UIManager.cpp"

} /* namespace CamGui */
