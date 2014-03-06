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

#include "../App/FeatureOrthoView.h"

#include "QGraphicsItemViewOrthographic.h"

using namespace DrawingGui;

QGraphicsItemViewOrthographic::QGraphicsItemViewOrthographic(const QPoint &pos, QGraphicsScene *scene)
    :QGraphicsItemViewCollection(pos, scene)
{
    this->setFlags(QGraphicsItem::ItemIsSelectable);
    this->setPos(pos);
    origin = new QGraphicsItemGroup();
    origin->setParentItem(this);


    m_backgroundItem = new QGraphicsRectItem();
    m_backgroundItem->setPen(QPen(QColor(Qt::black)));
    this->addToGroup(m_backgroundItem);
}

QGraphicsItemViewOrthographic::~QGraphicsItemViewOrthographic()
{

}

QVariant QGraphicsItemViewOrthographic::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(change == ItemChildAddedChange && scene()) {

        // Child Feature View has been added

        // TODO child must be intiailised before adding to containing parent
         QGraphicsItem *item = value.value<QGraphicsItem*>();

         QGraphicsItemView *view = 0;
         view = dynamic_cast<QGraphicsItemView *>(item);

         if(view) {
            Drawing::FeatureView *fView = view->getViewObject();
            if(fView->getTypeId().isDerivedFrom(Drawing::FeatureOrthoView::getClassTypeId())) {

                Drawing::FeatureOrthoView *orthoView = static_cast<Drawing::FeatureOrthoView *>(fView);
                QString type = QString::fromAscii(orthoView->Type.getValueAsString());
                if(type == QString::fromAscii("Top") ||
                   type == QString::fromAscii("Bottom") ||
                   type == QString::fromAscii("Back")) {
                    view->alignTo(origin, QString::fromAscii("Vertical"));
                } else if(type == QString::fromAscii("Front")) {
                    view->setLocked(true);
                } else {
                    view->alignTo(origin, QString::fromAscii("Horizontal"));
                }
            }

         }


    }

    return QGraphicsItemView::itemChange(change, value);
}

#if 0
QGraphicsItemViewOrthographic::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    QGraphicsItem::mousePressEvent(event);
    event->ignore();
}
#endif

void QGraphicsItemViewOrthographic::updateView(bool update)
{
             m_backgroundItem->setRect(this->boundingRect());
    return QGraphicsItemViewCollection::updateView(update);
}

#include "moc_QGraphicsItemViewOrthographic.cpp"
