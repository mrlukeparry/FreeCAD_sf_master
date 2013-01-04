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
# include <QFileInfo>
# include <QFileDialog>
# include <QGLWidget>
# include <QGraphicsScene>
# include <QGraphicsRectItem>
# include <QGraphicsSvgItem>
# include <QGridLayout>
# include <QGroupBox>
# include <QListWidget>
# include <QMenu>
# include <QMessageBox>
# include <QMouseEvent>
# include <QPainter>
# include <QPaintEvent>
# include <QPrinter>
# include <QPrintDialog>
# include <QPrintPreviewDialog>
# include <QPrintPreviewWidget>
# include <QScrollArea>
# include <QSlider>
# include <QStatusBar>
# include <QSvgRenderer>
# include <QSvgWidget>
# include <QWheelEvent>
# include <strstream>
# include <cmath>
#endif

#include <Base/Tools2D.h>
#include <Base/Stream.h>
#include <Base/gzstream.h>
#include <Base/PyObjectBase.h>
#include <Gui/FileDialog.h>
#include <Gui/WaitCursor.h>

#include "../App/Geometry.h"
#include "../App/FeaturePage.h"
#include "../App/FeatureViewPart.h"
#include "CanvasView.h"

using namespace DrawingGui;

CanvasView::CanvasView(QWidget *parent)
    : QGraphicsView(parent)
    , m_renderer(Native)
    , m_backgroundItem(0)
    , m_outlineItem(0)
{
    setScene(new QGraphicsScene(this));
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(ScrollHandDrag);

    // Prepare background check-board pattern
    QPixmap tilePixmap(64, 64);
    tilePixmap.fill(Qt::white);
    QPainter tilePainter(&tilePixmap);
    QColor color(220, 220, 220);
    tilePainter.fillRect(0, 0, 32, 32, color);
    tilePainter.fillRect(32, 32, 32, 32, color);
    tilePainter.end();

    setBackgroundBrush(tilePixmap);    
}

void CanvasView::drawBackground(QPainter *p, const QRectF &)
{
    p->save();
    p->resetTransform();
    p->drawTiledPixmap(viewport()->rect(), backgroundBrush().texture());
    p->restore();
}

void CanvasView::drawViewPart(Drawing::FeatureViewPart *part)
{
    // Iterate
    const std::vector<DrawingGeometry::BaseGeom *> &geoms = part->getGeometry();
    std::vector<DrawingGeometry::BaseGeom *>::const_iterator it = geoms.begin();
    QGraphicsScene *scene = this->scene();
    QGraphicsItemGroup *group = new QGraphicsItemGroup();
    
    // iterate through all the geometries
    for( ; it != geoms.end(); ++it) {
      switch((*it)->geomType) {
        case DrawingGeometry::CIRCLE: {
          DrawingGeometry::Circle *geom = static_cast<DrawingGeometry::Circle *>(*it);
          QGraphicsEllipseItem *item = new  QGraphicsEllipseItem();
          item->setRect(0, 0, geom->radius * 2, geom->radius * 2);
          item->setPos(geom->x - geom->radius, geom->y - geom->radius);
          group->addToGroup(item);
        } break;
        case DrawingGeometry::ARCOFCIRCLE: {
          DrawingGeometry::AOC  *geom = static_cast<DrawingGeometry::AOC *>(*it);
          QGraphicsPathItem *item = new  QGraphicsPathItem();
          QPainterPath path;

          double startAngle = (geom->startAngle);
          double spanAngle =  (geom->endAngle - startAngle);

          path.arcMoveTo(0, 0, geom->radius * 2, geom->radius * 2, -startAngle);
          path.arcTo(0, 0, geom->radius * 2, geom->radius * 2, -startAngle, -spanAngle);
          item->setPath(path);
          item->setPos(geom->x - geom->radius, geom->y - geom->radius);
          group->addToGroup(item);
        } break;
        case DrawingGeometry::ELLIPSE: {
          DrawingGeometry::Ellipse *geom = static_cast<DrawingGeometry::Ellipse *>(*it);
          QGraphicsEllipseItem *item = new  QGraphicsEllipseItem();
          item->setRect(0, 0, geom->major * 2, geom->minor * 2);
          item->setPos(geom->x - geom->radius, geom->y - geom->radius);
          group->addToGroup(item);
        } break;
        case DrawingGeometry::ARCOFELLIPSE: {
          DrawingGeometry::AOE *geom = static_cast<DrawingGeometry::AOE *>(*it);
          QGraphicsEllipseItem *item = new  QGraphicsEllipseItem();
          item->setRect(0, 0, geom->major * 2, geom->minor * 2);
          int startAngle = geom->startAngle * 16;
          int spanAngle = (geom->endAngle - geom->startAngle) * 16;
          item->setStartAngle(startAngle);
          item->setSpanAngle(spanAngle);
          item->setPos(geom->x, geom->y);
          group->addToGroup(item);
        } break;
        case DrawingGeometry::GENERIC: {
          DrawingGeometry::Generic  *geom = static_cast<DrawingGeometry::Generic *>(*it);
          QGraphicsPathItem *item = new  QGraphicsPathItem();
          QPainterPath path;
          path.moveTo(geom->points[0].fX, geom->points[0].fY);
          std::vector<Base::Vector2D>::const_iterator it = geom->points.begin();

          for(++it; it != geom->points.end(); ++it) {
              path.lineTo((*it).fX, (*it).fY);
          }
          item->setPath(path);
          group->addToGroup(item);
        } break;
        default:
          break;
      }
    }
    this->scene()->addItem(group);
}
void CanvasView::setRenderer(RendererType type)
{
    m_renderer = type;

    if (m_renderer == OpenGL) {
#ifndef QT_NO_OPENGL
        setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
#endif
    } else {
        setViewport(new QWidget);
    }
}

void CanvasView::setHighQualityAntialiasing(bool highQualityAntialiasing)
{
#ifndef QT_NO_OPENGL
    setRenderHint(QPainter::HighQualityAntialiasing, highQualityAntialiasing);
#else
    Q_UNUSED(highQualityAntialiasing);
#endif
}

void CanvasView::setViewBackground(bool enable)
{
    if (!m_backgroundItem)
        return;

    m_backgroundItem->setVisible(enable);
}

void CanvasView::setViewOutline(bool enable)
{
    if (!m_outlineItem)
        return;

    m_outlineItem->setVisible(enable);
}

void CanvasView::paintEvent(QPaintEvent *event)
{
    if (m_renderer == Image) {
        if (m_image.size() != viewport()->size()) {
            m_image = QImage(viewport()->size(), QImage::Format_ARGB32_Premultiplied);
        }

        QPainter imagePainter(&m_image);
        QGraphicsView::render(&imagePainter);
        imagePainter.end();

        QPainter p(viewport());
        p.drawImage(0, 0, m_image);

    } else {
        QGraphicsView::paintEvent(event);
    }
}

void CanvasView::wheelEvent(QWheelEvent *event)
{
    qreal factor = std::pow(1.2, -event->delta() / 240.0);
    scale(factor, factor);
    event->accept();
} 

#include "moc_CanvasView.cpp"