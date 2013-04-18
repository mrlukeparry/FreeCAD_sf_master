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

#ifndef DRAWINGGUI_QGRAPHICSITEMVIEWPART_H
#define DRAWINGGUI_QGRAPHICSITEMVIEWPART_H

# include <QObject>
# include <QGraphicsView>
# include <QPainter>
# include <QStyleOptionGraphicsItem>
# include "QGraphicsItemView.h"

namespace Drawing {
class FeatureViewPart;
}

namespace DrawingGeometry {
class BaseGeom;  
}

namespace DrawingGui
{
  
class QGraphicsItemEdge : public QObject, public QGraphicsPathItem
{
Q_OBJECT
public:
    explicit QGraphicsItemEdge(int ref = -1, QGraphicsScene *scene = 0 );

    ~QGraphicsItemEdge() {}
     QPainterPath shape() const;
    
    enum {Type = QGraphicsItem::UserType + 103};

    int type() const { return Type;}  
    int getReference() const { return reference; }
    
    void setHighlighted(bool state);
    void setShowHidden(bool state) {showHidden = state; update(); }
    QPainterPath getHiddenPath() { return hPath; }
    QPainterPath getVisiblePath() { return vPath; }
    void setStrokeWidth(float width) { strokeWidth = width; vPen.setWidthF(width); hPen.setWidthF(width * 0.3); update(); }
    
    bool contains(const QPointF &point);
    void setHiddenPath(const QPainterPath &path); 
    void setVisiblePath(const QPainterPath &path);
    
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
    
    QRectF boundingRect() const;
protected:
    // Preselection events:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    // Selection detection
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
  
protected:
  int reference;
  QPainterPath hPath;
  QPainterPath vPath;

  QBrush hBrush;
  QBrush vBrush;
  QPen   vPen;
  QPen   hPen;
  
  QRectF bb;
  bool highlighted;
  bool showHidden;
private:
float strokeWidth;

};

class QGraphicsItemFace : public QObject, public QGraphicsPathItem
{
Q_OBJECT
public:
    explicit QGraphicsItemFace(int ref = -1, QGraphicsScene *scene = 0 );

    ~QGraphicsItemFace() {}
//      QPainterPath shape() const;
    
    enum {Type = QGraphicsItem::UserType + 104};

    int type() const { return Type;}  
    int getReference() const { return reference; } 
    
protected:
    // Preselection events:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    // Selection detection
     QVariant itemChange(GraphicsItemChange change, const QVariant &value);
  
protected:
int reference;

private:
QPen m_pen;
};


class QGraphicsItemVertex : public QObject, public QGraphicsPathItem
{
Q_OBJECT
public:
    explicit QGraphicsItemVertex(int ref = -1, QGraphicsScene *scene = 0 );

    ~QGraphicsItemVertex() {}
     QPainterPath shape() const;
    
    enum {Type = QGraphicsItem::UserType + 105};

    int type() const { return Type;}  
    int getReference() const { return reference; } 
    void setStrokeWidth(int width) { this->strokeWidth = width; }
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
protected:
    // Preselection events:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    // Selection detection
     QVariant itemChange(GraphicsItemChange change, const QVariant &value);
  
protected:
int reference;

private:
int strokeWidth;
QPen m_pen;
};


class DrawingGuiExport QGraphicsItemViewPart : public QGraphicsItemView
{
    Q_OBJECT

public:
    enum {Type = QGraphicsItem::UserType + 102};
    explicit QGraphicsItemViewPart(const QPoint &position, QGraphicsScene *scene);
    ~QGraphicsItemViewPart();

    void setViewPartFeature(Drawing::FeatureViewPart *obj);
    int type() const { return Type;}
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

Q_SIGNALS:
  void dirty();
  
protected:
  QGraphicsItemEdge * findRefEdge(int i);
  QPainterPath drawPainterPath(DrawingGeometry::BaseGeom *baseGeom) const;
  void drawViewPart();
  void drawBorder(QPainter *painter);
  virtual QPainterPath  shape () const;
  virtual QRectF boundingRect() const; 
  // Selection detection
  virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
  void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
  QPen pen;
};

} // namespace DrawingViewGui

#endif // DRAWINGGUI_QGRAPHICSITEMVIEWPART_H