/***************************************************************************
 *   Copyright (c) 2012-2013 Luke Parry <l.parry@warwick.ac.uk>            *
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
# include <QContextMenuEvent>
# include <QGraphicsScene>
# include <QGraphicsSceneMouseEvent>
# include <QMenu>
# include <QMessageBox>
# include <QMouseEvent>
# include <QPainter>
# include <strstream>
#endif

#include <App/Document.h>
#include <Base/Console.h>
#include <Gui/Selection.h>
#include <Gui/Command.h>

#include "../App/FeatureViewCollection.h"
#include "QGraphicsItemViewCollection.h"

using namespace DrawingGui;

QGraphicsItemViewCollection::QGraphicsItemViewCollection(const QPoint &pos, QGraphicsScene *scene) :QGraphicsItemView(pos, scene)
{
    this->setFlags(QGraphicsItem::ItemIsSelectable);
    this->setPos(pos);

    setHandlesChildEvents(false);

    //setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
}

QGraphicsItemViewCollection::~QGraphicsItemViewCollection()
{

}

QVariant QGraphicsItemViewCollection::itemChange(GraphicsItemChange change, const QVariant &value)
{

    return QGraphicsItemGroup::itemChange(change, value);
}

void QGraphicsItemViewCollection::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    if(scene() && this == scene()->mouseGrabberItem()) {
        Gui::Command::openCommand("Drag Orthographic Collection");
        Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.X = %f", this->getViewObject()->getNameInDocument(), this->x());
        Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.Y = %f", this->getViewObject()->getNameInDocument(), this->getY());
        Gui::Command::commitCommand();
        //Gui::Command::updateActive();
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

void QGraphicsItemViewCollection::updateView(bool update)
{
    return QGraphicsItemView::updateView(update);
}

#include "moc_QGraphicsItemViewCollection.cpp"
