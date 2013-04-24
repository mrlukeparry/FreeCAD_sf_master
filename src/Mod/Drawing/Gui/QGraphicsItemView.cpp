/***************************************************************************
 *   Copyright (c) 2012 Luke Parry <l.parry@warwick.ac.uk>                 *
 *                                                                         *
 *   This file is Drawing of the FreeCAD CAx development system.           *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A DrawingICULAR PURPOSE.  See the      *
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
# include <QAction>
# include <QApplication>
# include <QContextMenuEvent>
# include <QGraphicsScene>
# include <QGraphicsSceneMouseEvent>
# include <QGridLayout>
# include <QMenu>
# include <QMessageBox>
# include <QMouseEvent>
# include <QPainter>
# include <strstream>
# include <cmath>
#endif

#include <App/Document.h>
#include <Base/Console.h>
#include <Gui/Selection.h>
#include <Gui/Command.h>
#include "../App/FeatureView.h"
#include "QGraphicsItemView.h"

using namespace DrawingGui;

QGraphicsItemView::QGraphicsItemView(const QPoint &pos, QGraphicsScene *scene) :QGraphicsItemGroup(),viewObject(0)                 
{
    this->setFlags(QGraphicsItem::ItemIsSelectable/* |
              QGraphicsItem::ItemSendsGeometryChanges |
              QGraphicsItem::ItemIsMovable |
              QGraphicsItem::ItemIsFocusable |
              QGraphicsItem::ItemSendsGeometryChanges*/);
    this->setPos(pos);
    
    //Add object to scene
    scene->addItem(this);
}

QGraphicsItemView::~QGraphicsItemView()
{
  
}

QVariant QGraphicsItemView::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch(change) {
//       case QGraphicsItem::ItemSelectedHasChanged: {
//         
//       } break;
      case QGraphicsItem::ItemPositionChange: {
          
              
      } break;
    }

    return QGraphicsItemGroup::itemChange(change, value);
}

void QGraphicsItemView::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{       
    if(scene() && this == scene()->mouseGrabberItem()) {
        Gui::Command::openCommand("Drag View");
        Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.X = %f", viewObject->getNameInDocument(), this->x());
        Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.Y = %f", viewObject->getNameInDocument(), this->y());
        Gui::Command::commitCommand();
        Gui::Command::updateActive();
    }
    QGraphicsItem::mouseReleaseEvent(event);   
}

void QGraphicsItemView::updateView()
{
    Base::Console().Log("Updating View");
    this->setPos(viewObject->X.getValue(), viewObject->Y.getValue());
}

void QGraphicsItemView::setViewFeature(Drawing::FeatureView *obj)
{
    if(obj == 0)
        return;

    this->viewObject = obj;
    Q_EMIT dirty();
}

#include "moc_QGraphicsItemView.cpp"