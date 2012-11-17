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

#ifndef TOOLPATHDOCKWINDOW_H_
#define TOOLPATHDOCKWINDOW_H_


#include <Gui/DockWindow.h>
#include <qtextedit.h>

namespace CamGui {

class ToolPathDockWindow : public Gui::DockWindow {

  Q_OBJECT

public:
  ToolPathDockWindow(Gui::Document*  pcDocument, QWidget *parent=0);
  virtual ~ToolPathDockWindow();

  /**
   * Set the Toolpath to be displayed.  It expects the toolpath to be in HTML
   * format already.
   */
  void setToolPath(const QString &toolpath);

protected:
  QTextEdit *textedit;
};

} /* namespace Cam */
#endif /* TOOLPATHDOCKWINDOW_H_ */
