/***************************************************************************
 *   Copyright (c) 2007 Jürgen Riegel <juergen.riegel@web.de>              *
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
# include <QBuffer>
# include <QContextMenuEvent>
# include <QFileInfo>
# include <QFileDialog>
# include <QGLWidget>
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

#include <Gui/Document.h>
#include <App/DocumentObject.h>
#include <Base/Console.h>
#include <Base/Stream.h>
#include <Base/gzstream.h>
#include <Base/PyObjectBase.h>

#include <Gui/FileDialog.h>
#include <Gui/WaitCursor.h>

#include "../App/FeaturePage.h"
#include "../App/FeatureViewPart.h"
#include "../App/FeatureViewSection.h"
#include "../App/FeatureViewDimension.h"

#include "QGraphicsItemView.h"
#include "QGraphicsItemViewPart.h"
#include "QGraphicsItemViewDimension.h"

#include "CanvasView.h"
#include "DrawingView.h"

using namespace DrawingGui;

#if 0
SvgView::SvgView(QWidget *parent)
    : QGraphicsView(parent)
    , m_renderer(Native)
    , m_svgItem(0)
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

void SvgView::drawBackground(QPainter *p, const QRectF &)
{
    p->save();
    p->resetTransform();
    p->drawTiledPixmap(viewport()->rect(), backgroundBrush().texture());
    p->restore();
}

void SvgView::openFile(const QFile &file)
{
    if (!file.exists())
        return;

    QGraphicsScene *s = scene();

    bool drawBackground = (m_backgroundItem ? m_backgroundItem->isVisible() : true);
    bool drawOutline = (m_outlineItem ? m_outlineItem->isVisible() : false);

    s->clear();
    resetTransform();

    m_svgItem = new QGraphicsSvgItem(file.fileName());
    m_svgItem->setFlags(QGraphicsItem::ItemClipsToShape);
    m_svgItem->setCacheMode(QGraphicsItem::NoCache);
    m_svgItem->setZValue(0);

    m_backgroundItem = new QGraphicsRectItem(m_svgItem->boundingRect());
    m_backgroundItem->setBrush(Qt::white);
    m_backgroundItem->setPen(Qt::NoPen);
    m_backgroundItem->setVisible(drawBackground);
    m_backgroundItem->setZValue(-1);

    m_outlineItem = new QGraphicsRectItem(m_svgItem->boundingRect());
    QPen outline(Qt::black, 2, Qt::DashLine);
    outline.setCosmetic(true);
    m_outlineItem->setPen(outline);
    m_outlineItem->setBrush(Qt::NoBrush);
    m_outlineItem->setVisible(drawOutline);
    m_outlineItem->setZValue(1);

    s->addItem(m_backgroundItem);
    s->addItem(m_svgItem);
    s->addItem(m_outlineItem);

    s->setSceneRect(m_outlineItem->boundingRect().adjusted(-10, -10, 10, 10));
}

void SvgView::setRenderer(RendererType type)
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

void SvgView::setHighQualityAntialiasing(bool highQualityAntialiasing)
{
#ifndef QT_NO_OPENGL
    setRenderHint(QPainter::HighQualityAntialiasing, highQualityAntialiasing);
#else
    Q_UNUSED(highQualityAntialiasing);
#endif
}

void SvgView::setViewBackground(bool enable)
{
    if (!m_backgroundItem)
        return;

    m_backgroundItem->setVisible(enable);
}

void SvgView::setViewOutline(bool enable)
{
    if (!m_outlineItem)
        return;

    m_outlineItem->setVisible(enable);
}

void SvgView::paintEvent(QPaintEvent *event)
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

void SvgView::wheelEvent(QWheelEvent *event)
{
    qreal factor = std::pow(1.2, -event->delta() / 240.0);
    scale(factor, factor);
    event->accept();
}
#endif
// ----------------------------------------------------------------------------

/* TRANSLATOR DrawingGui::DrawingView */

DrawingView::DrawingView(Gui::Document* doc, QWidget* parent)
  : Gui::MDIView(doc, parent), m_view(new CanvasView)
{
    m_backgroundAction = new QAction(tr("&Background"), this);
    m_backgroundAction->setEnabled(false);
    m_backgroundAction->setCheckable(true);
    m_backgroundAction->setChecked(true);
    connect(m_backgroundAction, SIGNAL(toggled(bool)), m_view, SLOT(setViewBackground(bool)));

    m_outlineAction = new QAction(tr("&Outline"), this);
    m_outlineAction->setEnabled(false);
    m_outlineAction->setCheckable(true);
    m_outlineAction->setChecked(false);
    connect(m_outlineAction, SIGNAL(toggled(bool)), m_view, SLOT(setViewOutline(bool)));

    m_nativeAction = new QAction(tr("&Native"), this);
    m_nativeAction->setCheckable(true);
    m_nativeAction->setChecked(false);
#ifndef QT_NO_OPENGL
    m_glAction = new QAction(tr("&OpenGL"), this);
    m_glAction->setCheckable(true);
#endif
    m_imageAction = new QAction(tr("&Image"), this);
    m_imageAction->setCheckable(true);

#ifndef QT_NO_OPENGL
    m_highQualityAntialiasingAction = new QAction(tr("&High Quality Antialiasing"), this);
    m_highQualityAntialiasingAction->setEnabled(false);
    m_highQualityAntialiasingAction->setCheckable(true);
    m_highQualityAntialiasingAction->setChecked(false);
    connect(m_highQualityAntialiasingAction, SIGNAL(toggled(bool)),
            m_view, SLOT(setHighQualityAntialiasing(bool)));
#endif

    QActionGroup *rendererGroup = new QActionGroup(this);
    rendererGroup->addAction(m_nativeAction);
#ifndef QT_NO_OPENGL
    rendererGroup->addAction(m_glAction);
#endif
    rendererGroup->addAction(m_imageAction);
    connect(rendererGroup, SIGNAL(triggered(QAction *)),
            this, SLOT(setRenderer(QAction *)));

    setCentralWidget(m_view);
    //setWindowTitle(tr("SVG Viewer"));

    // Connect Signals and Slots
    QObject::connect(
        m_view->scene(), SIGNAL(selectionChanged()),
        this           , SLOT  (selectionChanged())
       );
}

DrawingView::~DrawingView()
{

}

void DrawingView::attachPageObject(Drawing::FeaturePage *pageFeature)
{
    // A fresh page is added and we iterate through its collected children and add these to Canvas View
    const std::vector<App::DocumentObject*> &grp = pageFeature->Views.getValues();
    for (std::vector<App::DocumentObject*>::const_iterator it = grp.begin();it != grp.end(); ++it) {
        attachView(*it);
    }

    // Save a link to the page feature - exclusivly one page per drawing view
    pageFeat.setValue(dynamic_cast<App::DocumentObject*>(pageFeature));
}

void DrawingView::attachView(App::DocumentObject *obj)
{
    if(obj->getTypeId().isDerivedFrom(Drawing::FeatureViewSection::getClassTypeId()) ) {
        Drawing::FeatureViewSection *viewSect = dynamic_cast<Drawing::FeatureViewSection *>(obj);
        m_view->addViewSection(viewSect);
    } else if (obj->getTypeId().isDerivedFrom(Drawing::FeatureViewPart::getClassTypeId()) ) {
        Drawing::FeatureViewPart *viewPart = dynamic_cast<Drawing::FeatureViewPart *>(obj);
        m_view->addViewPart(viewPart);
    } else if(obj->getTypeId().isDerivedFrom(Drawing::FeatureViewDimension::getClassTypeId()) ) {
        Drawing::FeatureViewDimension *viewDim = dynamic_cast<Drawing::FeatureViewDimension *>(obj);
        m_view->addViewDimension(viewDim);
    }
}

void DrawingView::preSelectionChanged(const QPoint &pos)
{
    QObject *obj = QObject::sender();

    if(!obj)
        return;

    // Check if an edge was preselected
    QGraphicsItemEdge *edge   = dynamic_cast<QGraphicsItemEdge *>(obj);
    QGraphicsItemVertex *vert = dynamic_cast<QGraphicsItemVertex *>(obj);
    if(edge) {

        // Find the parent view that this edges is contained within
        QGraphicsItem *parent = edge->parentItem();
        if(!parent)
            return;

        QGraphicsItemView *viewItem = dynamic_cast<QGraphicsItemView *>(parent);
        if(!viewItem)
          return;

        Drawing::FeatureView *viewObj = viewItem->getViewObject();
        std::stringstream ss;
        ss << "Edge" << edge->getReference();
        bool accepted =
        Gui::Selection().setPreselect(viewObj->getDocument()->getName()
                                     ,viewObj->getNameInDocument()
                                     ,ss.str().c_str()
                                     ,pos.x()
                                     ,pos.y()
                                     ,0);

    } else if(vert) {
              // Find the parent view that this edges is contained within
        QGraphicsItem *parent = vert->parentItem();
        if(!parent)
            return;

        QGraphicsItemView *viewItem = dynamic_cast<QGraphicsItemView *>(parent);
        if(!viewItem)
          return;

        Drawing::FeatureView *viewObj = viewItem->getViewObject();
        std::stringstream ss;
        ss << "Edge" << vert->getReference();
        bool accepted =
        Gui::Selection().setPreselect(viewObj->getDocument()->getName()
                                     ,viewObj->getNameInDocument()
                                     ,ss.str().c_str()
                                     ,pos.x()
                                     ,pos.y()
                                     ,0);
    } else {
            // Check if an edge was preselected
        QGraphicsItemView *view = qobject_cast<QGraphicsItemView *>(obj);

        if(!view)
            return;
        Drawing::FeatureView *viewObj = view->getViewObject();
        Gui::Selection().setPreselect(viewObj->getDocument()->getName()
                                     ,viewObj->getNameInDocument()
                                     ,""
                                     ,pos.x()
                                     ,pos.y()
                                     ,0);
    }
}

void DrawingView::selectionChanged()
{
    QList<QGraphicsItem *> selection = m_view->scene()->selectedItems();

    bool block = this->blockConnection(true); // avoid to be notified by itself
    Gui::Selection().clearSelection();
    for (QList<QGraphicsItem *>::iterator it = selection.begin(); it != selection.end(); ++it) {
        // All selectable items must be of QGraphicsItemView type

        QGraphicsItemView *itemView = dynamic_cast<QGraphicsItemView *>(*it);
        if(itemView == 0) {
            QGraphicsItemEdge *edge = dynamic_cast<QGraphicsItemEdge *>(*it);
            if(edge) {

                // Find the parent view that this edges is contained within
                QGraphicsItem *parent = edge->parentItem();
                if(!parent)
                    return;

                QGraphicsItemView *viewItem = dynamic_cast<QGraphicsItemView *>(parent);
                if(!viewItem)
                  return;

                Drawing::FeatureView *viewObj = viewItem->getViewObject();

                std::stringstream ss;
                ss << "Edge" << edge->getReference();
                bool accepted =
                Gui::Selection().addSelection(viewObj->getDocument()->getName()
                                            ,viewObj->getNameInDocument()
                                            ,ss.str().c_str());
            }

            QGraphicsItemVertex *vert = dynamic_cast<QGraphicsItemVertex *>(*it);
            if(vert) {
              // Find the parent view that this edges is contained within
                QGraphicsItem *parent = vert->parentItem();
                if(!parent)
                    return;

                QGraphicsItemView *viewItem = dynamic_cast<QGraphicsItemView *>(parent);
                if(!viewItem)
                  return;

                Drawing::FeatureView *viewObj = viewItem->getViewObject();

                std::stringstream ss;
                ss << "Vertex" << vert->getReference();
                bool accepted =
                Gui::Selection().addSelection(viewObj->getDocument()->getName()
                                            ,viewObj->getNameInDocument()
                                            ,ss.str().c_str());

            }

            QGraphicsItemDatumLabel *dimLabel = dynamic_cast<QGraphicsItemDatumLabel*>(*it);
            if(dimLabel) {
                // Find the parent view (dimLabel->dim->view)

                QGraphicsItem *dimParent = dimLabel->parentItem();

                if(!dimParent)
                    return;

                QGraphicsItemView *dimItem = dynamic_cast<QGraphicsItemView *>(dimParent);

                if(!dimItem)
                  return;

                Drawing::FeatureView *dimObj = dimItem->getViewObject();

                bool accepted =
                Gui::Selection().addSelection(dimObj->getDocument()->getName(),dimObj->getNameInDocument());

            }
            continue;

        }

        Drawing::FeatureView *viewObj = itemView->getViewObject();

        std::string doc_name = viewObj->getDocument()->getName();
        std::string obj_name = viewObj->getNameInDocument();

        Gui::Selection().addSelection(doc_name.c_str(), obj_name.c_str());
    }
    this->blockConnection(block);

    QList<QGraphicsItem *> addSelection;
    QList<QGraphicsItem *> remSelection;
#if 0
    for(QList<QGraphicsItem *>::const_iterator it = selection.begin(); it != selection.end(); ++it) {
        bool found = false;
        for(QList<QGraphicsItem *>::const_iterator pit = prevSelection.begin(); pit != prevSelection.end(); ++pit) {
            if((*it) == (*pit)) {
                found = true;
                prevSelection.removeOne(*pit);
                break;
            }
        }
        if(!found)
            addSelection.push_back(*it);

    }

    // Any remaining entries must have be removed
    remSelection = prevSelection;


    QGraphicsItemGroup *group = qgraphicsitem_cast<QGraphicsItemGroup *>(items.at(0));
    if(group == NULL)
        return;
    bool selected = group->isSelected();

    QList<QGraphicsItem *> children = group->children();
    for(QList<QGraphicsItem *>::const_iterator it = children.begin(); it != children.end(); ++it) {
      QAbstractGraphicsShapeItem *item = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(*it);
      QPen pen = item->pen();
      if(selected)
          pen.setColor(SelectColor);
      else
          pen.setColor(QColor(0, 0,0));
      item->setPen(pen);
    }

    // Cache the selection
    prevSelection.clear();
    prevSelection = scene()->selectedItems();
#endif
}

void DrawingView::updateDrawing()
{
    // We cannot guarantee if the number of views have changed so check the number
    const std::vector<QGraphicsItemView *> &views = m_view->getViews();
    Drawing::FeaturePage *pageFeature = dynamic_cast<Drawing::FeaturePage *>(pageFeat.getValue());
    std::vector<App::DocumentObject*> grp = pageFeature->Views.getValues();

    //
    if(views.size() < grp.size()) {
        // An  view object has been added so update graphicsviews
        std::list<App::DocumentObject*> notFnd;
        // Iterate through to find any views that are missing
        std::vector<QGraphicsItemView *>::const_iterator qview = views.begin();

        // TODO think of a better algorithm to deal with any changes to views list
        // Find any additions
        for(std::vector<App::DocumentObject*>::const_iterator it = grp.begin(); it != grp.end(); ++it) {
            bool fnd = false;
            while(qview != views.end()) {

                // Unsure if we can compare pointers so rely on name
                if(strcmp((*qview)->getViewObject()->getNameInDocument(), (*it)->getNameInDocument()) == 0) {
                    fnd = true;
                    break;
                }
                qview++;
            }

            if(!fnd)
                notFnd.push_back(*it);
        }

         // Iterate over missing views and add them
        for(std::list<App::DocumentObject*>::const_iterator it = notFnd.begin(); it != notFnd.end(); ++it) {
            attachView(*it);
        }

    } else if(views.size() > grp.size()) {
        // A View Object has been removed
        std::vector<QGraphicsItemView *>::const_iterator qview = views.begin();
        bool fnd = false;

        // Updated QItemView List
        std::vector<QGraphicsItemView *> myViews;

        // Remove any orphans
        while(qview != views.end()) {
            fnd = false;
            for(std::vector<App::DocumentObject*>::const_iterator it = grp.begin(); it != grp.end(); ++it) {

                // Unsure if we can compare pointers so rely on name
                if(strcmp((*qview)->getViewObject()->getNameInDocument(), (*it)->getNameInDocument()) == 0) {
                    fnd = true;
                    myViews.push_back(*qview);
                    break;
                }
            }

            if(!fnd) {
                m_view->scene()->removeItem(*qview);
            }
            qview++;
        }

        // Update the canvas view list of QGraphicsItemViews
        m_view->setViews(myViews);
    }

    // Updated all the views
    const std::vector<QGraphicsItemView *> &upviews = m_view->getViews();
    for(std::vector<QGraphicsItemView *>::const_iterator it = upviews.begin(); it != upviews.end(); ++it) {
        (*it)->updateView();
    }

}

void DrawingView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;
    menu.addAction(this->m_backgroundAction);
    menu.addAction(this->m_outlineAction);
    QMenu* submenu = menu.addMenu(tr("&Renderer"));
    submenu->addAction(this->m_nativeAction);
    submenu->addAction(this->m_glAction);
    submenu->addAction(this->m_imageAction);
    submenu->addSeparator();
    submenu->addAction(this->m_highQualityAntialiasingAction);
    menu.exec(event->globalPos());
}

void DrawingView::setRenderer(QAction *action)
{
#ifndef QT_NO_OPENGL
    m_highQualityAntialiasingAction->setEnabled(false);
#endif

    if (action == m_nativeAction)
        m_view->setRenderer(CanvasView::Native);
#ifndef QT_NO_OPENGL
    else if (action == m_glAction) {
        m_highQualityAntialiasingAction->setEnabled(true);
        m_view->setRenderer(CanvasView::OpenGL);
    }
#endif
    else if (action == m_imageAction) {
        m_view->setRenderer(CanvasView::Image);
    }
}

void DrawingView::onSelectionChanged(const Gui::SelectionChanges& msg)
{
    if (msg.Type == Gui::SelectionChanges::ClrSelection) {

    }
    else if (msg.Type == Gui::SelectionChanges::AddSelection ||
             msg.Type == Gui::SelectionChanges::RmvSelection) {
        bool select = (msg.Type == Gui::SelectionChanges::AddSelection);
        // Check if it is a view object
    }
    else if (msg.Type == Gui::SelectionChanges::SetSelection) {
        // do nothing here
    }
}

bool DrawingView::onMsg(const char* pMsg, const char** ppReturn)
{
    if (strcmp("ViewFit",pMsg) == 0) {
        viewAll();
        return true;
    } else if (strcmp("Redo", pMsg) == 0 ) {
        getAppDocument()->redo();
    } else if (strcmp("Undo", pMsg) == 0 ) {
        getAppDocument()->undo();
    }
    return false;
}

bool DrawingView::onHasMsg(const char* pMsg) const
{
    if (strcmp("ViewFit",pMsg) == 0)
        return true;
    else if(strcmp("Redo", pMsg) == 0 && getAppDocument()->getAvailableRedos() > 0)
        return true;
    else if(strcmp("Undo", pMsg) == 0 && getAppDocument()->getAvailableUndos() > 0)
        return true;
    else if (strcmp("Print",pMsg) == 0)
        return true;
    else if (strcmp("PrintPreview",pMsg) == 0)
        return true;
    else if (strcmp("PrintPdf",pMsg) == 0)
        return true;
    return false;
}

void DrawingView::printPdf()
{
    Gui::FileOptionsDialog dlg(this, 0);
    dlg.setFileMode(QFileDialog::AnyFile);
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    dlg.setWindowTitle(tr("Export PDF"));
    dlg.setFilters(QStringList() << tr("PDF file (*.pdf)"));

    QGridLayout *gridLayout;
    QGridLayout *formLayout;
    QGroupBox *groupBox;
    QListWidget *listWidget;
    QListWidgetItem* item;
    QWidget *form = new QWidget(&dlg);
    form->resize(40, 300);
    formLayout = new QGridLayout(form);
    groupBox = new QGroupBox(form);
    gridLayout = new QGridLayout(groupBox);
    listWidget = new QListWidget(groupBox);
    gridLayout->addWidget(listWidget, 0, 0, 1, 1);
    formLayout->addWidget(groupBox, 0, 0, 1, 1);

    groupBox->setTitle(tr("Page sizes"));
    item = new QListWidgetItem(tr("A0"), listWidget);
    item->setData(Qt::UserRole, QVariant(QPrinter::A0));
    item = new QListWidgetItem(tr("A1"), listWidget);
    item->setData(Qt::UserRole, QVariant(QPrinter::A1));
    item = new QListWidgetItem(tr("A2"), listWidget);
    item->setData(Qt::UserRole, QVariant(QPrinter::A2));
    item = new QListWidgetItem(tr("A3"), listWidget);
    item->setData(Qt::UserRole, QVariant(QPrinter::A3));
    item = new QListWidgetItem(tr("A4"), listWidget);
    item->setData(Qt::UserRole, QVariant(QPrinter::A4));
    item = new QListWidgetItem(tr("A5"), listWidget);
    item->setData(Qt::UserRole, QVariant(QPrinter::A5));
    listWidget->item(4)->setSelected(true); // by default A4
    dlg.setOptionsWidget(Gui::FileOptionsDialog::ExtensionRight, form, false);

    if (dlg.exec() == QDialog::Accepted) {
        Gui::WaitCursor wc;
        QString filename = dlg.selectedFiles().front();
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(filename);
        printer.setOrientation(QPrinter::Landscape);
        QList<QListWidgetItem*> items = listWidget->selectedItems();
        if (items.size() == 1) {
            int AX = items.front()->data(Qt::UserRole).toInt();
            printer.setPaperSize(QPrinter::PageSize(AX));
        }
        print(&printer);
    }
}

void DrawingView::print()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);
    printer.setOrientation(QPrinter::Landscape);
    QPrintDialog dlg(&printer, this);
    if (dlg.exec() == QDialog::Accepted) {
        print(&printer);
    }
}

void DrawingView::printPreview()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);
    printer.setOrientation(QPrinter::Landscape);

    QPrintPreviewDialog dlg(&printer, this);
    connect(&dlg, SIGNAL(paintRequested (QPrinter *)),
            this, SLOT(print(QPrinter *)));
    dlg.exec();
}

void DrawingView::print(QPrinter* printer)
{
#if 1
    QPainter p(printer);
    QRect rect = printer->pageRect();
    this->m_view->scene()->render(&p, rect);
    p.end();
#else
    printer->setResolution(QPrinter::HighResolution);
    printer->setPageSize(QPrinter::A4);
    QPainter painter(printer);

    // print, fitting the viewport contents into a full page
    m_view->render(&painter);

    // print the upper half of the viewport into the lower.
    // half of the page.
    QRect viewport = m_view->viewport()->rect();
    m_view->render(&painter,
                   QRectF(0, printer->height() / 2,
                             printer->width(), printer->height() / 2),
                   viewport.adjusted(0, 0, 0, -viewport.height() / 2));
#endif
}

void DrawingView::viewAll()
{
    m_view->fitInView(m_view->scene()->sceneRect(), Qt::KeepAspectRatio);
}

PyObject* DrawingView::getPyObject()
{
    Py_Return;
}

#include "moc_DrawingView.cpp"