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

#ifndef DRAWINGGUI_CANVASVIEW_H
#define DRAWINGGUI_CANVASVIEW_H

#include <Gui/MDIView.h>
#include <QGraphicsView>

QT_BEGIN_NAMESPACE
class QSlider;
class QAction;
class QActionGroup;
class QFile;
class QGraphicsItemGroup;
class QPopupMenu;
class QToolBar;
class QSvgWidget;
class QScrollArea;
class QPrinter;
QT_END_NAMESPACE

namespace Drawing {
class FeatureViewPart;  
}

namespace DrawingGeometry
{
class BaseGeom;  
}

namespace DrawingGui
{
  
class DrawingGuiExport CanvasView : public QGraphicsView
{
    Q_OBJECT

public:
    enum RendererType { Native, OpenGL, Image };

    CanvasView(QWidget *parent = 0);
    ~CanvasView() {}

    void setRenderer(RendererType type = Native);
    void drawBackground(QPainter *p, const QRectF &rect);
    void drawViewPart(Drawing::FeatureViewPart *part);

public Q_SLOTS:
    void setHighQualityAntialiasing(bool highQualityAntialiasing);
    void setViewBackground(bool enable);
    void setViewOutline(bool enable);

protected:
    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);
    

private:
    RendererType m_renderer;

    QGraphicsRectItem *m_backgroundItem;
    QGraphicsRectItem *m_outlineItem;

    QImage m_image;
};

} // namespace DrawingViewGui

#endif // DRAWINGGUI_DRAWINGVIEW_H