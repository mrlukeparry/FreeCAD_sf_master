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

#ifndef DRAWINGGUI_QGRAPHICSITEMVIEW_H
#define DRAWINGGUI_QGRAPHICSITEMVIEW_H

#include <QGraphicsItemGroup>
#include <QObject>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

namespace Drawing {
class FeatureView;
}

namespace DrawingGui
{

class DrawingGuiExport  QGraphicsItemView : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT

public:
    QGraphicsItemView(const QPoint &position, QGraphicsScene *scene);
    ~QGraphicsItemView();

    enum {Type = QGraphicsItem::UserType + 101};
    int type() const { return Type;}

    void setViewFeature(Drawing::FeatureView *obj);
    Drawing::FeatureView * getViewObject() const { return viewObject; }

    virtual void updateView(bool update = false);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

Q_SIGNALS:
  void dirty();

protected:
  virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
  Drawing::FeatureView *viewObject;
};

} // namespace DrawingViewGui

#endif // DRAWINGGUI_QGRAPHICSITEMVIEW_H