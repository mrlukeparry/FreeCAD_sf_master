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
# include <QList>
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

#include <Mod/Drawing/App/FeatureOrthoView.h>
#include <Mod/Drawing/App/FeatureViewOrthographic.h>

#include "QGraphicsItemViewOrthographic.h"

using namespace DrawingGui;

QGraphicsItemViewOrthographic::QGraphicsItemViewOrthographic(const QPoint &pos, QGraphicsScene *scene)
    :QGraphicsItemViewCollection(pos, scene)
{
    this->setPos(pos);
    origin = new QGraphicsItemGroup();
    origin->setParentItem(this);

    // In place to ensure correct drawing and bounding box calculations
    m_backgroundItem = new QGraphicsRectItem();
    //m_backgroundItem->setPen(QPen(QColor(Qt::black)));
    //this->addToGroup(m_backgroundItem);
    setFlag(ItemIsSelectable, false);
    setFlag(ItemIsMovable, true);
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
                    // Get FeatureViewOrthohraphic object
                    App::DocumentObject *obj = this->getViewObject();
                    Drawing::FeatureViewOrthographic *viewOrthographic = dynamic_cast<Drawing::FeatureViewOrthographic *>(obj);

                    viewOrthographic->Anchor.setValue(fView);
                    updateView();
                } else {
                    view->alignTo(origin, QString::fromAscii("Horizontal"));
                }
            }

         }
    }

    return QGraphicsItemView::itemChange(change, value);
}


void QGraphicsItemViewOrthographic::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  QGraphicsItem::mousePressEvent(event);
}

void QGraphicsItemViewOrthographic::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{

    QGraphicsItem::mouseMoveEvent(event);
}

QGraphicsItemView * QGraphicsItemViewOrthographic::getAnchorQItem() const
{
        // Get FeatureViewOrthohraphic object
    App::DocumentObject *obj = this->getViewObject();
    Drawing::FeatureViewOrthographic *viewOrthographic = dynamic_cast<Drawing::FeatureViewOrthographic *>(obj);

      // Get the currently assigned anchor view
    App::DocumentObject *anchorObj = viewOrthographic->Anchor.getValue();
    Drawing::FeatureView *anchorView = dynamic_cast<Drawing::FeatureView *>(anchorObj);

    // Locate the anchor view's qgraphicsitemview
    QList<QGraphicsItem *> list =  this->childItems();

    for (QList<QGraphicsItem *>::iterator it = list.begin(); it != list.end(); ++it) {
        QGraphicsItemView *view = dynamic_cast<QGraphicsItemView *>(*it);
        if(view && strcmp(view->getViewName(), anchorView->getNameInDocument()) == 0) {
              return view;
        }
    }
    return 0;
}

void QGraphicsItemViewOrthographic::updateView(bool update)
{
    m_backgroundItem->setRect(this->boundingRect());
    return QGraphicsItemViewCollection::updateView(update);
}

#include "moc_QGraphicsItemViewOrthographic.cpp"
