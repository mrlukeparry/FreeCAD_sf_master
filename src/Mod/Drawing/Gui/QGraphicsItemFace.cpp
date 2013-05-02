/***************************************************************************
 *   Copyright (c) 2013 Luke Parry <l.parry@warwick.ac.uk>                 *
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
#include <QAction>
#include <QApplication>
#include <QContextMenuEvent>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QPainterPathStroker>
#include <QPainter>
#endif

#include "QGraphicsItemFace.h"

using namespace DrawingGui;

QGraphicsItemFace::QGraphicsItemFace(int ref, QGraphicsScene *scene  ) : reference(ref)
{
    if(scene) {
        scene->addItem(this);
    }
    this->setAcceptHoverEvents(true);
}

QVariant QGraphicsItemFace::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedHasChanged && scene()) {
        // value is the new position.
        if(isSelected()) {
            QPen pen = this->pen();
            pen.setColor(Qt::blue);
            this->setPen(pen);
        } else {
            QPen pen = this->pen();
            pen.setColor(Qt::gray);
            this->setPen(pen);
        }
        update();
    }
    return QGraphicsItem::itemChange(change, value);
}

void QGraphicsItemFace::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QPen pen = this->pen();
    pen.setColor(Qt::blue);
    this->setPen(pen);
    update();
}

void QGraphicsItemFace::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItemView *view = dynamic_cast<QGraphicsItemView *> (this->parentItem());
    assert(view != 0);

    if(!isSelected() && !view->isSelected()) {
        QPen pen = this->pen();
        pen.setColor(Qt::black);
        this->setPen(pen);
        update();
    }
}

#include "moc_QGraphicsItemFace.cpp"