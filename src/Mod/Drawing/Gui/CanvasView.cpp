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
#include "../App/FeatureViewDimension.h"

#include "QGraphicsItemViewPart.h"
#include "QGraphicsItemViewDimension.h"
#include "CanvasView.h"

using namespace DrawingGui;
QColor CanvasView::PreselectColor   =    QColor(0.88f,0.88f,0.0f);   // #E1E100 -> (225,225,  0)
QColor CanvasView::SelectColor      =    QColor(0.11f,0.68f,0.11f);  // #1CAD1C -> ( 28,173, 28)

CanvasView::CanvasView(QWidget *parent)
    : QGraphicsView(parent)
    , m_renderer(Native)
    , m_backgroundItem(0)
    , m_outlineItem(0)
{
    setScene(new QGraphicsScene(this));
    setTransformationAnchor(AnchorUnderMouse);
//     setDragMode(ScrollHandDrag);

    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
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

void CanvasView::addViewPart(Drawing::FeatureViewPart *part)
{   
    QGraphicsItemViewPart *group = new QGraphicsItemViewPart(QPoint(0,0), this->scene());
    group->setViewPartFeature(part);
    
    views.push_back(group); 
}

void CanvasView::addViewDimension(Drawing::FeatureViewDimension *dim)
{   
    QGraphicsItemViewDimension *dimGroup = new QGraphicsItemViewDimension(QPoint(0,0), this->scene());
    dimGroup->setViewPartFeature(dim);
    std::vector<App::DocumentObject *> objs = dim->References.getValues();
    
    if(objs.size() > 0)
    {
        // Attach the dimension to the first object's group
        for(std::vector<QGraphicsItemView *>::iterator it = views.begin(); it != views.end(); ++it) {
            Drawing::FeatureView *viewObj = (*it)->getViewObject();
            if(viewObj == objs.at(0)) {
                // Found dimensions parent view reference
                QGraphicsItemView *view = (*it);
                dimGroup->setPos(view->pos()); 
                view->addToGroup(dimGroup);
                //Must transfer to new coordinate system of parent view
                
                return;
            }
        }
    }
    
    // The dimensions parent couldn't be found. Add for now.
    views.push_back(dimGroup); 
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