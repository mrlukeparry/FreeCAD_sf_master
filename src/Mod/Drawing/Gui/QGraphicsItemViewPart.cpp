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
#include <QGridLayout>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainterPathStroker>
#include <QPainter>
#include <strstream>
#include <cmath>
#endif

#include <Base/Console.h>
#include "../App/FeatureViewPart.h"
#include "QGraphicsItemViewPart.h"

using namespace DrawingGui;

QGraphicsItemEdge::QGraphicsItemEdge(int ref, QGraphicsScene *scene  ) : reference(ref) 
{
    if(scene) {
        scene->addItem(this);
    }
    this->setAcceptHoverEvents(true);    
}

QPainterPath QGraphicsItemEdge::shape() const
{
    QPainterPathStroker stroker;
    stroker.setWidth(this->pen().widthF());
    return stroker.createStroke(this->path());
}

QVariant QGraphicsItemEdge::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedHasChanged && scene()) {
        // value is the new position.
        if(isSelected()) {
          QPen pen = this->pen();
          pen.setColor(Qt::blue);
          this->setPen(pen);
        } else {
          QPen pen = this->pen();
          pen.setColor(Qt::black);
          this->setPen(pen);
        }
        update();
    }
    return QGraphicsItem::itemChange(change, value);
}
 
void QGraphicsItemEdge::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QPen pen = this->pen();
    pen.setColor(Qt::blue);
    this->setPen(pen);
    update();
}

void QGraphicsItemEdge::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
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

// QGraphicsView - Face Features

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

QGraphicsItemViewPart::QGraphicsItemViewPart(const QPoint &pos, QGraphicsScene *scene) :QGraphicsItemView(pos, scene)                 
{
  setHandlesChildEvents(false);
}

QGraphicsItemViewPart::~QGraphicsItemViewPart()
{
  
}

void QGraphicsItemViewPart::drawViewPart()
{    
    
    // Iterate
    if(this->viewObject == 0 || !this->viewObject->isDerivedFrom(Drawing::FeatureViewPart::getClassTypeId()))
        return;
        
    Drawing::FeatureViewPart *part = dynamic_cast<Drawing::FeatureViewPart *>(this->viewObject);
   
    float lineWidth = part->LineWidth.getValue();

    QPen pen;
    pen.setWidthF((int) lineWidth);
    pen.setStyle(Qt::SolidLine);
    
    // Draw Edges
    const std::vector<DrawingGeometry::BaseGeom *> &geoms = part->getEdgeGeometry();
    const std::vector<int> &refs = part->getEdgeReferences();
    std::vector<DrawingGeometry::BaseGeom *>::const_iterator it = geoms.begin();
    
    QGraphicsItem *graphicsItem = 0;
    
    // Draw Faces 
    
    const std::vector<DrawingGeometry::BaseGeom *> &faceGeoms = part->getFaceGeometry();
    const std::vector<int> &faceRefs = part->getFaceReferences();
    
    std::vector<DrawingGeometry::BaseGeom *>::const_iterator fit = faceGeoms.begin();
    
    QPen facePen;
    for(int i = 0 ; fit != faceGeoms.end(); ++fit, i++) {
        DrawingGeometry::Generic *geom = static_cast<DrawingGeometry::Generic *>(*fit);
        QGraphicsItemFace *item = new  QGraphicsItemFace(-1);
        
//         item->setStrokeWidth(lineWidth);
        facePen.setBrush(QBrush(Qt::BDiagPattern));
        graphicsItem = dynamic_cast<QGraphicsItem *>(item);

        QPainterPath path;
        path.moveTo(geom->points[0].fX, geom->points[0].fY);
        std::vector<Base::Vector2D>::const_iterator it = geom->points.begin();

        for(++it; it != geom->points.end(); ++it) {
            path.lineTo((*it).fX, (*it).fY);
        }
        item->setPen(facePen);
        item->setPath(path);
        
        if(graphicsItem) {          
            // Hide any edges that are hidden if option is set.
//             if((*fit)->extractType == DrawingGeometry::WithHidden && !part->ShowHiddenLines.getValue())
//                 graphicsItem->hide();
              
            this->addToGroup(graphicsItem);
            graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
        }      
    }
    
    graphicsItem = 0;
    return;
    // Draw Edges      
    // iterate through all the geometries
    for(int i = 0 ; it != geoms.end(); ++it, i++) {
      if((*it)->extractType == DrawingGeometry::WithHidden)
          pen.setStyle(Qt::DashLine);
      else
          pen.setStyle(Qt::SolidLine);

      switch((*it)->geomType) {
        case DrawingGeometry::CIRCLE: {
          DrawingGeometry::Circle *geom = static_cast<DrawingGeometry::Circle *>(*it);
          QGraphicsItemEdge *item = new QGraphicsItemEdge(refs.at(i));
          
          item->setStrokeWidth(lineWidth);
          
          graphicsItem = dynamic_cast<QGraphicsItem *>(item);
          
          QPainterPath path;
          path.addEllipse(0 ,0, geom->radius * 2, geom->radius * 2);
          item->setPen(pen);
          item->setPath(path);
          item->setPos(geom->x - geom->radius, geom->y - geom->radius);
          
          graphicsItem = dynamic_cast<QGraphicsItem *>(item);

          item->setPen(pen);

        } break;
        case DrawingGeometry::ARCOFCIRCLE: {
          DrawingGeometry::AOC  *geom = static_cast<DrawingGeometry::AOC *>(*it);
          QGraphicsItemEdge *item = new QGraphicsItemEdge(refs.at(i));
          
          item->setStrokeWidth(lineWidth);
          
          graphicsItem = dynamic_cast<QGraphicsItem *>(item);
          QPainterPath path;

          double startAngle = (geom->startAngle);
          double spanAngle =  (geom->endAngle - startAngle);

          path.arcMoveTo(0, 0, geom->radius * 2, geom->radius * 2, -startAngle);
          path.arcTo(0, 0, geom->radius * 2, geom->radius * 2, -startAngle, -spanAngle);

          item->setPen(pen);
          item->setPath(path);

          item->setPos(geom->x - geom->radius, geom->y - geom->radius);

        } break;
        case DrawingGeometry::ELLIPSE: {
          DrawingGeometry::Ellipse *geom = static_cast<DrawingGeometry::Ellipse *>(*it);
          
          QGraphicsItemEdge *item = new QGraphicsItemEdge(refs.at(i));
          
          item->setStrokeWidth(lineWidth);
          
          graphicsItem = dynamic_cast<QGraphicsItem *>(item);

          QPainterPath path;

          path.addEllipse(0,0, geom->major * 2, geom->minor * 2);

          item->setPen(pen);
          item->setPath(path);

          item->setPos(geom->x - geom->radius, geom->y - geom->radius);

        } break;
        case DrawingGeometry::ARCOFELLIPSE: {
          DrawingGeometry::AOE *geom = static_cast<DrawingGeometry::AOE *>(*it);
          QGraphicsItemEdge *item = new QGraphicsItemEdge(refs.at(i));
          
          item->setStrokeWidth(lineWidth);
          
          graphicsItem = dynamic_cast<QGraphicsItem *>(item);
          
          QPainterPath path;

          double startAngle = (geom->startAngle);
          double spanAngle =  (startAngle - geom->endAngle);
          double endAngle = geom->endAngle;

          Base::Console().Log("SA %f, EA %f, SA %f \n", -startAngle, endAngle, spanAngle);
          path.arcMoveTo(0, 0, geom->major * 2, geom->minor * 2, -startAngle);
          path.arcTo(0, 0, geom->major * 2, geom->minor * 2, -startAngle, spanAngle);
          item->setPath(path);
          
          item->setTransformOriginPoint(geom->major, geom->minor);
          item->setRotation(geom->angle);          
          item->setPos(geom->x - geom->major, geom->y - geom->minor);
          
          item->setPen(pen);

        } break;
        case DrawingGeometry::BSPLINE: {
          DrawingGeometry::BSpline *geom = static_cast<DrawingGeometry::BSpline *>(*it);
          QGraphicsItemEdge *item = new  QGraphicsItemEdge(refs.at(i));
          
          item->setStrokeWidth(lineWidth);
          
          graphicsItem = dynamic_cast<QGraphicsItem *>(item);

          QPainterPath path;
          
          std::vector<DrawingGeometry::BezierSegment>::const_iterator it = geom->segments.begin();
          
          DrawingGeometry::BezierSegment startSeg = geom->segments.at(0);
          path.moveTo(startSeg.pnts[0].fX, startSeg.pnts[0].fY);
          Base::Vector2D prevContPnt = startSeg.pnts[1];
          
          for(int i = 0; it != geom->segments.end(); ++it, ++i) {
              DrawingGeometry::BezierSegment seg = *it;
              if(seg.poles == 4) {
//                   path.cubicTo(seg.pnts[1].fX,seg.pnts[1].fY, seg.pnts[2].fX, seg.pnts[2].fY, seg.pnts[3].fX, seg.pnts[3].fY);
              } else {
                  Base::Vector2D cPnt;
                  if(i == 0) {
                    prevContPnt.Set(startSeg.pnts[1].fX, startSeg.pnts[1].fX);
                  } else {
                    prevContPnt.Set(2 * startSeg.pnts[1].fX - prevContPnt.fX, 2 * startSeg.pnts[1].fY - prevContPnt.fY);
                  }

                  path.quadTo(prevContPnt.fX, prevContPnt.fY, seg.pnts[2].fX, seg.pnts[2].fY);

              }
            }
          item->setPen(pen);
          item->setPath(path);
          
        } break;
        case DrawingGeometry::GENERIC: {
          DrawingGeometry::Generic *geom = static_cast<DrawingGeometry::Generic *>(*it);
          QGraphicsItemEdge *item = new  QGraphicsItemEdge(refs.at(i));
          
          item->setStrokeWidth(lineWidth);
          
          graphicsItem = dynamic_cast<QGraphicsItem *>(item);

          QPainterPath path;
          path.moveTo(geom->points[0].fX, geom->points[0].fY);
          std::vector<Base::Vector2D>::const_iterator it = geom->points.begin();

          for(++it; it != geom->points.end(); ++it) {
              path.lineTo((*it).fX, (*it).fY);
          }
          item->setPen(pen);
          item->setPath(path);

        } break;
        default:
          break;
      }
      
      if(graphicsItem) {
        
          // Hide any edges that are hidden if option is set.
          if((*it)->extractType == DrawingGeometry::WithHidden && !part->ShowHiddenLines.getValue())
              graphicsItem->hide();
            
          this->addToGroup(graphicsItem);
          graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
      }
    }
}

QVariant QGraphicsItemViewPart::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedHasChanged && scene()) {
        // value is the new position.
          QColor color;
        if(isSelected()) { 
          color.setRgb(0,0,255);

        } else {
          color.setRgb(0,0,0);
        }
        
        QList<QGraphicsItem *> items = this->childItems();
          for(QList<QGraphicsItem *>::iterator it = items.begin(); it != items.end(); ++it) {
              QGraphicsItemEdge *edge = dynamic_cast<QGraphicsItemEdge *>(*it);
              QPen pen = edge->pen();
              pen.setColor(color);
              edge->setPen(pen);              
          }
          
        update();
    }
    return QGraphicsItemView::itemChange(change, value);
}

void QGraphicsItemViewPart::setViewPartFeature(Drawing::FeatureViewPart *obj)
{
    if(obj == 0)
        return;
    
    this->setViewFeature(static_cast<Drawing::FeatureView *>(obj));
    this->drawViewPart();
      // Set the QGraphicsItemGroup Properties based on the FeatureView
    float x = obj->X.getValue();
    float y = obj->Y.getValue();
    float scale = obj->Scale.getValue();
    this->setPos(x, y);
    this->setScale(scale);
    Q_EMIT dirty();
}

#include "moc_QGraphicsItemViewPart.cpp"