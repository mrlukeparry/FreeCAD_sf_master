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

#ifndef DRAWINGGUI_QGRAPHICSITEMVIEWDIMENSION_H
#define DRAWINGGUI_QGRAPHICSITEMVIEWDIMENSION_H

#include <QObject>
#include <QGraphicsView>
#include <QStyleOptionGraphicsItem>
#include "QGraphicsItemView.h"

namespace Drawing {
class FeatureViewDimension;
}

namespace DrawingGeometry {
class BaseGeom;
}

namespace DrawingGui
{

class QGraphicsItemDatumLabel : public QGraphicsTextItem
{
Q_OBJECT
public:
    explicit QGraphicsItemDatumLabel(int ref = -1, QGraphicsScene *scene = 0 );
    ~QGraphicsItemDatumLabel() {}

    void updatePos();
    enum {Type = QGraphicsItem::UserType + 107};
    int type() const { return Type;}

    void setPosition(const double &x, const double &y);
    double X() const { return posX; }
    double Y() const { return posY; }

    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

Q_SIGNALS:
  void dragging();
  void hover(bool state);
  void selected(bool state);
  void dragFinished();

protected:
    // Preselection events:
    void mouseReleaseEvent( QGraphicsSceneMouseEvent * event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    // Selection detection
     QVariant itemChange(GraphicsItemChange change, const QVariant &value);

protected:
int reference;
double posX;
double posY;

private:
int strokeWidth;
QPen m_pen;
};

class DrawingGuiExport QGraphicsItemViewDimension : public QGraphicsItemView
{
    Q_OBJECT

public:
    enum {Type = QGraphicsItem::UserType + 106};

    explicit QGraphicsItemViewDimension(const QPoint &position, QGraphicsScene *scene);
    ~QGraphicsItemViewDimension();

    void setViewPartFeature(Drawing::FeatureViewDimension *obj);
    int type() const { return Type;}

    virtual void updateView(bool update = false);
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

Q_SIGNALS:
  void dirty();

public Q_SLOTS:
  void datumLabelDragged(void);
  void datumLabelDragFinished(void);
  void select(bool state);
  void hover(bool state);
  void updateDim(void);

protected:
  void draw();
  void clearProjectionCache();
    // Selection detection
  virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

protected:
  bool hasHover;
  QGraphicsItem *datumLabel;
  QGraphicsItem *arrows;
  QGraphicsItem *centreLines;

  std::vector<QGraphicsItem *> arw;
  std::vector<DrawingGeometry::BaseGeom *> projGeom;
  QPen pen;
};

} // namespace DrawingViewGui

#endif // DRAWINGGUI_QGRAPHICSITEMVIEWDIMENSION_H