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

#ifndef MACHINEPROGRAMDOCKWINDOW_H_
#define MACHINEPROGRAMDOCKWINDOW_H_


#include <Gui/DockWindow.h>
#include <QTextEdit>

#include "ui_MachineProgramDockWindow.h"

#include "../App/Features/ToolPathFeature.h"
#include "../App/Features/MachineProgramFeature.h"

namespace CamGui {

class MachineProgramDockWindow : public Gui::DockWindow {

  Q_OBJECT

public:
  MachineProgramDockWindow(Gui::Document*  pcDocument, QWidget *parent=0);
  virtual ~MachineProgramDockWindow();

  /**
   * Set the MachineProgram to be displayed.
   */
  void setMachineProgram(Cam::MachineProgram *machine_program);

  /**
   * Clear the output (i.e. no toolpath or machine program selected)
   */
  void clearSelection();

public Q_SLOTS:
    /**
     * Receive messages to update the toolpath display
     */
    void updatedToolPathSelection(Cam::ToolPathFeature* toolpath);

    /**
     * Receive messages to update the machineProgram display
     */
    void updatedMachineProgramSelection(Cam::MachineProgramFeature* machineProgram);

protected:
//  QTextEdit *textedit;

private:
    Ui_MachineProgramDockWindow* ui;
	Cam::MachineProgram *machine_program;
};

} /* namespace Cam */
#endif /* MACHINEPROGRAMDOCKWINDOW_H_ */
