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

// Vertex Features
QGraphicsItemVertex::QGraphicsItemVertex(int ref, QGraphicsScene *scene  ) : reference(ref) 
{
    if(scene) {
        scene->addItem(this);
    }
    this->setAcceptHoverEvents(true);    
}

QPainterPath QGraphicsItemVertex::shape() const
{
    QPainterPathStroker stroker;
    stroker.setWidth(this->pen().widthF());
    return stroker.createStroke(this->path());
}

QVariant QGraphicsItemVertex::itemChange(GraphicsItemChange change, const QVariant &value)
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
 
void QGraphicsItemVertex::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QPen pen = this->pen();
    pen.setColor(Qt::blue);
    this->setPen(pen);
    update();
}

void QGraphicsItemVertex::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
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

QPainterPath QGraphicsItemViewPart::drawPainterPath(DrawingGeometry::BaseGeom *baseGeom) const
{
    QPainterPath path;
    switch(baseGeom->geomType) {
        case DrawingGeometry::CIRCLE: {
          DrawingGeometry::Circle *geom = static_cast<DrawingGeometry::Circle *>(baseGeom);
          
          double x = geom->center.fX - geom->radius;
          double y = geom->center.fY - geom->radius;
          
          path.addEllipse(x, y, geom->radius * 2, geom->radius * 2);

        } break;
        case DrawingGeometry::ARCOFCIRCLE: {
          DrawingGeometry::AOC  *geom = static_cast<DrawingGeometry::AOC *>(baseGeom);

          double startAngle = (geom->startAngle);
          double spanAngle =  (geom->endAngle - startAngle);

          double x = geom->center.fX - geom->radius;
          double y = geom->center.fY - geom->radius;
          
          path.arcMoveTo(x, y, geom->radius * 2, geom->radius * 2, -startAngle);
          path.arcTo(x, y, geom->radius * 2, geom->radius * 2, -startAngle, -spanAngle);
        } break;
        case DrawingGeometry::ELLIPSE: {
          DrawingGeometry::Ellipse *geom = static_cast<DrawingGeometry::Ellipse *>(baseGeom);

          double x = geom->center.fX - geom->radius;
          double y = geom->center.fY - geom->radius;
          
          path.addEllipse(x,y, geom->major * 2, geom->minor * 2);
        } break;
        case DrawingGeometry::ARCOFELLIPSE: {
          DrawingGeometry::AOE *geom = static_cast<DrawingGeometry::AOE *>(baseGeom);
          
          double startAngle = (geom->startAngle);
          double spanAngle =  (startAngle - geom->endAngle);
          double endAngle = geom->endAngle;
          
          double x = geom->center.fX - geom->major;
          double y = geom->center.fY - geom->minor;
          
          double maj = geom->major * 2 * cos(geom->angle * M_PI / 180);
          double min = geom->minor * 2 * sin(geom->angle * M_PI / 180);

          path.arcMoveTo(x, y, maj, min, startAngle);
          path.arcTo(x, y, maj, min, startAngle, spanAngle);

        } break;
        case DrawingGeometry::BSPLINE: {
          DrawingGeometry::BSpline *geom = static_cast<DrawingGeometry::BSpline *>(baseGeom);
         
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
        } break;
        case DrawingGeometry::GENERIC: {
          DrawingGeometry::Generic *geom = static_cast<DrawingGeometry::Generic *>(baseGeom);          

          path.moveTo(geom->points[0].fX, geom->points[0].fY);
          std::vector<Base::Vector2D>::const_iterator it = geom->points.begin();

          for(++it; it != geom->points.end(); ++it) {
              path.lineTo((*it).fX, (*it).fY);
          }
        } break;
        default:
          break;
      }
      return path;
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
          
    QGraphicsItem *graphicsItem = 0;
    
#if 0
    // Draw Faces     
    const std::vector<DrawingGeometry::Face *> &faceGeoms = part->getFaceGeometry();
    const std::vector<int> &faceRefs = part->getFaceReferences();
    
    std::vector<DrawingGeometry::Face *>::const_iterator fit = faceGeoms.begin();
    
    QPen facePen;
    for(int i = 0 ; fit != faceGeoms.end(); ++fit, i++) {
        std::vector<DrawingGeometry::Wire *> faceWires = (*fit)->wires;
        QPainterPath facePath;
        for(std::vector<DrawingGeometry::Wire *>::iterator wire = faceWires.begin(); wire != faceWires.end(); ++wire) {
            QPainterPath wirePath;            
            for(std::vector<DrawingGeometry::BaseGeom *>::iterator baseGeom = (*wire)->geoms.begin(); baseGeom != (*wire)->geoms.end(); ++baseGeom) {
                wirePath.connectPath(drawPainterPath(*baseGeom));
            }
            wirePath.closeSubpath();
            facePath.addPath(wirePath);
        }
        
        QGraphicsItemFace *item = new QGraphicsItemFace(-1);
 
        item->setPath(facePath);        
//         item->setStrokeWidth(lineWidth);
        
        QBrush faceBrush(QBrush(QColor(0,0,255,40)));
        
        item->setBrush(faceBrush);
        facePen.setColor(Qt::black);
        item->setPen(facePen);
        graphicsItem = dynamic_cast<QGraphicsItem *>(item);
               
        if(graphicsItem) {          
            // Hide any edges that are hidden if option is set.
//             if((*fit)->extractType == DrawingGeometry::WithHidden && !part->ShowHiddenLines.getValue())
//                 graphicsItem->hide();
              
            this->addToGroup(graphicsItem);
            graphicsItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
        }      
    }
#endif

    graphicsItem = 0;
    // Draw Vertexs:
    const std::vector<DrawingGeometry::Vertex *> &verts = part->getVertexGeometry();
    const std::vector<int> &vertRefs = part->getVertexReferences();
    
    std::vector<DrawingGeometry::Vertex *>::const_iterator vert = verts.begin();
    
     // iterate through all the geometries
    for(int i = 0 ; vert != verts.end(); ++vert, i++) {
          DrawingGeometry::Vertex *myVertex = *vert;
          QGraphicsItemVertex *item = new QGraphicsItemVertex(i);
          QPainterPath path;
          QBrush faceBrush(QBrush(QColor(0,0,255,255)));
        
          item->setBrush(faceBrush);
          path.addEllipse(0 ,0, 4, 4);
          item->setPath(path);
          item->setPos(myVertex->pnt.fX - 2, myVertex->pnt.fY - 2);
          
          graphicsItem = dynamic_cast<QGraphicsItem *>(item);
          this->addToGroup(graphicsItem);
    }
    
    graphicsItem = 0;
        
    // Draw Edges
    const std::vector<DrawingGeometry::BaseGeom *> &geoms = part->getEdgeGeometry();
    const std::vector<int> &refs = part->getEdgeReferences();
    std::vector<DrawingGeometry::BaseGeom *>::const_iterator it = geoms.begin();
    
    // Draw Edges      
    // iterate through all the geometries
    for(int i = 0 ; it != geoms.end(); ++it, i++) {
      if((*it)->extractType == DrawingGeometry::WithHidden)
          pen.setStyle(Qt::DashLine);
      else
          pen.setStyle(Qt::SolidLine);
       // Attempt to find if a previous edge exists
      QGraphicsItemEdge *item = this->findRefEdge(refs.at(i));
          
      QPainterPath path;
      
      if(!item) {
          item = new QGraphicsItemEdge(refs.at(i));
          item->setStrokeWidth(lineWidth);
      } else {
          path = item->path();
      }

      switch((*it)->geomType) {
        case DrawingGeometry::CIRCLE: {
          DrawingGeometry::Circle *geom = static_cast<DrawingGeometry::Circle *>(*it);

          graphicsItem = dynamic_cast<QGraphicsItem *>(item);

          path.addEllipse(0 ,0, geom->radius * 2, geom->radius * 2);
          item->setPen(pen);
          item->setPath(path);
          item->setPos(geom->center.fX - geom->radius, geom->center.fY - geom->radius);
          
          graphicsItem = dynamic_cast<QGraphicsItem *>(item);

          item->setPen(pen);

        } break;
        case DrawingGeometry::ARCOFCIRCLE: {
          DrawingGeometry::AOC  *geom = static_cast<DrawingGeometry::AOC *>(*it);
          
          graphicsItem = dynamic_cast<QGraphicsItem *>(item);

          double startAngle = (geom->startAngle);
          double spanAngle =  (geom->endAngle - startAngle);

          path.arcMoveTo(0, 0, geom->radius * 2, geom->radius * 2, -startAngle);
          path.arcTo(0, 0, geom->radius * 2, geom->radius * 2, -startAngle, -spanAngle);

          item->setPen(pen);
          item->setPath(path);

          item->setPos(geom->center.fX - geom->radius, geom->center.fY - geom->radius);

        } break;
        case DrawingGeometry::ELLIPSE: {
          DrawingGeometry::Ellipse *geom = static_cast<DrawingGeometry::Ellipse *>(*it);
        
          graphicsItem = dynamic_cast<QGraphicsItem *>(item);

          path.addEllipse(0,0, geom->major * 2, geom->minor * 2);

          item->setPen(pen);
          item->setPath(path);

          item->setPos(geom->center.fX - geom->radius, geom->center.fY - geom->radius);

        } break;
        case DrawingGeometry::ARCOFELLIPSE: {
          DrawingGeometry::AOE *geom = static_cast<DrawingGeometry::AOE *>(*it);
                    
          graphicsItem = dynamic_cast<QGraphicsItem *>(item);

          double startAngle = (geom->startAngle);
          double spanAngle =  (geom->endAngle - geom->startAngle);
          double endAngle = geom->endAngle;

          Base::Console().Log("(C <%f, %f> rot %f, SA %f, EA %f, SA %f \n",
          geom->center.fX - geom->major, 
          geom-> center.fY - geom->minor,
          geom->angle, startAngle, endAngle, spanAngle);
          path.arcMoveTo(0, 0, geom->major * 2, geom->minor * 2, startAngle);
          path.arcTo(0, 0, geom->major * 2, geom->minor * 2, startAngle, spanAngle);
          item->setPath(path);
          
          item->setTransformOriginPoint(geom->major, geom->minor);
          item->setRotation(geom->angle);          
          item->setPos(geom->center.fX - geom->major, geom-> center.fY - geom->minor);
          
          item->setPen(pen);

        } break;
        case DrawingGeometry::BSPLINE: {
          DrawingGeometry::BSpline *geom = static_cast<DrawingGeometry::BSpline *>(*it);
                   
          graphicsItem = dynamic_cast<QGraphicsItem *>(item);

          QPainterPath path;
          
          std::vector<DrawingGeometry::BezierSegment>::const_iterator it = geom->segments.begin();
          
          DrawingGeometry::BezierSegment startSeg = geom->segments.at(0);
          path.moveTo(startSeg.pnts[0].fX, startSeg.pnts[0].fY);
          Base::Vector2D prevContPnt = startSeg.pnts[1];
          
          for(int i = 0; it != geom->segments.end(); ++it, ++i) {
              DrawingGeometry::BezierSegment seg = *it;
              if(seg.poles == 4) {
                   path.cubicTo(seg.pnts[1].fX,seg.pnts[1].fY, seg.pnts[2].fX, seg.pnts[2].fY, seg.pnts[3].fX, seg.pnts[3].fY);
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
                    
          graphicsItem = dynamic_cast<QGraphicsItem *>(item);
          
          path.moveTo(geom->points[0].fX, geom->points[0].fY);
          std::vector<Base::Vector2D>::const_iterator it = geom->points.begin();

          for(++it; it != geom->points.end(); ++it) {
              path.lineTo((*it).fX, (*it).fY);
          }
          item->setPen(pen);
          item->setPath(path);

        } break;
        default:
          delete item;
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

QGraphicsItemEdge * QGraphicsItemViewPart::findRefEdge(int idx)
{
    QList<QGraphicsItem *> items = this->childItems();
    for(QList<QGraphicsItem *>::iterator it = items.begin(); it != items.end(); it++) {
        QGraphicsItemEdge *edge = dynamic_cast<QGraphicsItemEdge *>(*it);
        if(edge && edge->getReference() == idx) {
            return edge;
        }
    }
    return 0;
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
              if(edge) {
                  QPen pen = edge->pen();
                  pen.setColor(color);
                  edge->setPen(pen);
              } else {
                  QGraphicsItemVertex *vert = dynamic_cast<QGraphicsItemVertex *>(*it);
              }
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