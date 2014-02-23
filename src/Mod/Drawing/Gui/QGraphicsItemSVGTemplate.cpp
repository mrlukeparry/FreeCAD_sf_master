/***************************************************************************
 *   Copyright (c) 2012-2014 Luke Parry <l.parry@warwick.ac.uk>            *
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
#include <QFile>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
# include <strstream>
#endif

#include <Base/Exception.h>
#include <Gui/Command.h>

#include <Mod/Drawing/App/Geometry.h>
#include <Mod/Drawing/App/FeatureSVGTemplate.h>

#include "QGraphicsItemSVGTemplate.h"

using namespace DrawingGui;

QGraphicsItemSVGTemplate::QGraphicsItemSVGTemplate(QGraphicsScene *scene) : QGraphicsItemTemplate(scene)
{

    m_svgRender = new QSvgRenderer();

    m_svgItem = new QGraphicsSvgItem();
    m_svgItem->setSharedRenderer(m_svgRender);

    m_svgItem->setFlags(QGraphicsItem::ItemClipsToShape);
    m_svgItem->setCacheMode(QGraphicsItem::DeviceCoordinateCache);

    this->addToGroup(m_svgItem);
}

QGraphicsItemSVGTemplate::~QGraphicsItemSVGTemplate()
{
  delete m_svgRender;
  delete m_svgItem;
  m_svgRender = 0;
  m_svgItem   = 0;
}

QVariant QGraphicsItemSVGTemplate::itemChange(GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsItemGroup::itemChange(change, value);
}


void QGraphicsItemSVGTemplate::clearContents()
{

}

void QGraphicsItemSVGTemplate::openFile(const QFile &file)
{

}

void QGraphicsItemSVGTemplate::load (const QString & fileName)
{
    if (fileName.isEmpty()){
        return;
    }

    QFile file(fileName);
    if (!file.exists()) {
        return;
    }
    m_svgRender->load(file.fileName());

    QSize size = m_svgRender->defaultSize();
    //Base::Console().Log("size of svg document <%i,%i>", size.width(), size.height());
    m_svgItem->setSharedRenderer(m_svgRender);

    Drawing::FeatureSVGTemplate *tmplte = getSVGTemplate();

    double xaspect, yaspect;
    xaspect = tmplte->getWidth() / (double) size.width();
    yaspect = tmplte->getHeight() / (double) size.height();

    QTransform qtrans;
    qtrans.translate(0.f, -tmplte->getHeight());
    qtrans.scale(xaspect , yaspect);
    m_svgItem->setTransform(qtrans);

}
Drawing::FeatureSVGTemplate * QGraphicsItemSVGTemplate::getSVGTemplate()
{
    if(pageTemplate && pageTemplate->isDerivedFrom(Drawing::FeatureSVGTemplate::getClassTypeId()))
        return static_cast<Drawing::FeatureSVGTemplate *>(pageTemplate);
    else
        return 0;
}

void QGraphicsItemSVGTemplate::draw()
{

    Drawing::FeatureSVGTemplate *tmplte = getSVGTemplate();
    if(!tmplte)
        throw Base::Exception("Template Feuature not set for QGraphicsItemSVGTemplate");

    this->load(QString::fromUtf8(tmplte->PageResult.getValue()));

}

void QGraphicsItemSVGTemplate::updateView(bool update)
{
    draw();
}

#include "moc_QGraphicsItemSVGTemplate.cpp"
