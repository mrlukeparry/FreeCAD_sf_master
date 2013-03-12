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

#ifndef CAM_UIMANAGER_H_
#define CAM_UIMANAGER_H_

#include <QObject>
#include <QListView>

#include "../App/TPG/TPGFactory.h"
#include "TPGListModel.h"

namespace CamGui {


/**
 * A class to manage the interactions between the various UI components of the
 * CAM workbench.
 */
class CamGuiExport UIManagerInst : public QObject {

  Q_OBJECT

protected:
  static UIManagerInst* _pcSingleton;

public:
  UIManagerInst();
  virtual ~UIManagerInst();

  // singleton manipators
  static UIManagerInst& instance(void);
  static void destruct (void);

public Q_SLOTS:
  void addTPG(Cam::TPGDescriptor *tpg);
  void reloadTPGs();

Q_SIGNALS:
  void updatedTPGList(TPGListModel *model);

  void updatedTPGSelection(Cam::TPG* tpg);
};

/// Get the global instance
inline UIManagerInst& UIManager(void)
{
    return UIManagerInst::instance();
}

} /* namespace CamGui */
#endif /* CAMGUI_UIMANAGER_H_ */
