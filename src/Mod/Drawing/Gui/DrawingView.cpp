/***************************************************************************
 *   Copyright (c) 2007 Juergen Riegel <juergen.riegel@web.de>              *
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
# include <QSvgGenerator>
# include <QSvgWidget>
# include <QWheelEvent>
# include <strstream>
# include <cmath>
#endif

#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/Window.h>
#include <Gui/MainWindow.h>
#include <Gui/Selection.h>
#include <App/DocumentObject.h>
#include <Base/Console.h>
#include <Base/Stream.h>
#include <Base/gzstream.h>
#include <Base/PyObjectBase.h>

#include <Gui/FileDialog.h>
#include <Gui/WaitCursor.h>

#include <Mod/Drawing/App/FeaturePage.h>
#include <Mod/Drawing/App/FeatureViewOrthographic.h>
#include <Mod/Drawing/App/FeatureViewPart.h>
#include <Mod/Drawing/App/FeatureViewSection.h>
#include <Mod/Drawing/App/FeatureViewDimension.h>
#include <Mod/Drawing/App/FeatureTemplate.h>

#include "QGraphicsItemDrawingTemplate.h"
#include "QGraphicsItemView.h"
#include "QGraphicsItemViewPart.h"
#include "QGraphicsItemViewDimension.h"
#include "ViewProviderPage.h"
#include "CanvasView.h"
#include "DrawingView.h"


using namespace DrawingGui;

/* TRANSLATOR DrawingGui::DrawingView */

DrawingView::DrawingView(ViewProviderDrawingPage *pageVp, Gui::Document* doc, QWidget* parent)
  : Gui::MDIView(doc, parent), pageGui(pageVp)
{

  // Setup the Canvas View
    m_view = new CanvasView(pageVp);

    m_backgroundAction = new QAction(tr("&Background"), this);
    m_backgroundAction->setEnabled(false);
    m_backgroundAction->setCheckable(true);
    m_backgroundAction->setChecked(true);
    connect(m_backgroundAction, SIGNAL(toggled(bool)), m_view, SLOT(setViewBackground(bool)));

    m_exportSVGAction = new QAction(tr("&Export SVG"), this);
    connect(m_exportSVGAction, SIGNAL(triggered()), this, SLOT(saveSVG()));

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

    isSlectionBlocked = false;

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


     // A fresh page is added and we iterate through its collected children and add these to Canvas View
    const std::vector<App::DocumentObject*> &grp = pageGui->getPageObject()->Views.getValues();
    for (std::vector<App::DocumentObject*>::const_iterator it = grp.begin();it != grp.end(); ++it) {
        attachView(*it);
    }

    App::DocumentObject *obj = pageGui->getPageObject()->Template.getValue();
    if(obj && obj->isDerivedFrom(Drawing::FeatureTemplate::getClassTypeId())) {
        Drawing::FeatureTemplate *pageTemplate = dynamic_cast<Drawing::FeatureTemplate *>(obj);
        this->attachTemplate(pageTemplate);
    }

}

DrawingView::~DrawingView()
{
  delete m_view;
  m_view = 0;
}

void DrawingView::attachPageObject(Drawing::FeaturePage *pageFeature)
{
    return;

    // redundant;

#if 0
    // A fresh page is added and we iterate through its collected children and add these to Canvas View
    const std::vector<App::DocumentObject*> &grp = pageFeature->Views.getValues();
    for (std::vector<App::DocumentObject*>::const_iterator it = grp.begin();it != grp.end(); ++it) {
        attachView(*it);
    }

    // Save a link to the page feature - exclusivly one page per drawing view
    pageFeat.setValue(dynamic_cast<App::DocumentObject*>(pageFeature));

    App::DocumentObject *obj = pageFeature->Template.getValue();
    if(obj && obj->isDerivedFrom(Drawing::FeatureTemplate::getClassTypeId())) {
        Drawing::FeatureTemplate *pageTemplate = dynamic_cast<Drawing::FeatureTemplate *>(obj);
        this->attachTemplate(pageTemplate);
    }
#endif
}

void DrawingView::attachTemplate(Drawing::FeatureTemplate *obj)
{
    m_view->setPageTemplate(obj);
}

int DrawingView::attachView(App::DocumentObject *obj)
{
    QGraphicsItemView *qview = 0;
    if(obj->getTypeId().isDerivedFrom(Drawing::FeatureViewSection::getClassTypeId()) ) {
        Drawing::FeatureViewSection *viewSect = dynamic_cast<Drawing::FeatureViewSection *>(obj);
        qview = m_view->addViewSection(viewSect);
    } else if (obj->getTypeId().isDerivedFrom(Drawing::FeatureViewPart::getClassTypeId()) ) {
        Drawing::FeatureViewPart *viewPart = dynamic_cast<Drawing::FeatureViewPart *>(obj);
        qview = m_view->addViewPart(viewPart);
    } else if (obj->getTypeId().isDerivedFrom(Drawing::FeatureViewOrthographic::getClassTypeId()) ) {
        Drawing::FeatureViewOrthographic *view = dynamic_cast<Drawing::FeatureViewOrthographic *>(obj);
        qview = m_view->addViewOrthographic(view);
    } else if (obj->getTypeId().isDerivedFrom(Drawing::FeatureViewCollection::getClassTypeId()) ) {
        Drawing::FeatureViewCollection *collection = dynamic_cast<Drawing::FeatureViewCollection *>(obj);
        qview =  m_view->addFeatureView(collection);
    } else if(obj->getTypeId().isDerivedFrom(Drawing::FeatureViewDimension::getClassTypeId()) ) {
        Drawing::FeatureViewDimension *viewDim = dynamic_cast<Drawing::FeatureViewDimension *>(obj);
        qview = m_view->addViewDimension(viewDim);
    }

    if(!qview)
        return -1;
    else
        return m_view->getViews().size();
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

void DrawingView::blockSelection(const bool state)
{
  this->isSlectionBlocked = state;
}

void DrawingView::clearSelection()
{
  this->blockSelection(true);
  std::vector<QGraphicsItemView *> views = m_view->getViews();

  // Iterate through all views and unselect all
  for (std::vector<QGraphicsItemView *>::iterator it = views.begin(); it != views.end(); ++it) {
      QGraphicsItemView *item = *it;
      item->setSelected(false);
      item->updateView();
  }

  this->blockSelection(false);
}

void DrawingView::selectFeature(App::DocumentObject *obj, const bool isSelected)
{
    // Update CanvasView's selection based on Selection made outside Drawing Interace
  QGraphicsItemView *view = m_view->findView(obj);

  this->blockSelection(true);
  if(view) {
      view->setSelected(isSelected);
      view->updateView();
  }
  this->blockSelection(false);
}

void DrawingView::selectionChanged()
{
    if(isSlectionBlocked)
      return;

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
                Gui::Selection().addSelection(viewObj->getDocument()->getName(),
                                              viewObj->getNameInDocument(),
                                              ss.str().c_str());
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
                Gui::Selection().addSelection(viewObj->getDocument()->getName(),
                                              viewObj->getNameInDocument(),
                                              ss.str().c_str());

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

        } else {

            Drawing::FeatureView *viewObj = itemView->getViewObject();

            std::string doc_name = viewObj->getDocument()->getName();
            std::string obj_name = viewObj->getNameInDocument();

            Gui::Selection().addSelection(doc_name.c_str(), obj_name.c_str());
        }

    }

    this->blockConnection(block);
}

void DrawingView::updateTemplate(bool forceUpdate)
{
    App::DocumentObject *templObj = pageGui->getPageObject()->Template.getValue();
    if(pageGui->getPageObject()->Template.isTouched() || templObj->isTouched()) {
        // Template is touched so update

        if(forceUpdate ||
           (templObj && templObj->isTouched() && templObj->isDerivedFrom(Drawing::FeatureTemplate::getClassTypeId())) ) {

            QGraphicsItemTemplate *qItemTemplate = m_view->getTemplate();

            if(qItemTemplate) {
                Drawing::FeatureTemplate *pageTemplate = dynamic_cast<Drawing::FeatureTemplate *>(templObj);
                qItemTemplate->setTemplate(pageTemplate);
                qItemTemplate->updateView();
            }
        }
    }

    // m_view->setPageFeature(pageFeature); redundant
}

void DrawingView::updateDrawing()
{
    // We cannot guarantee if the number of views have changed so check the number
    const std::vector<QGraphicsItemView *> &views = m_view->getViews();
    const std::vector<App::DocumentObject*> &grp  = pageGui->getPageObject()->Views.getValues();


    // Count total number of children
    int groupCount = 0;

    /*
    for(std::vector<App::DocumentObject*>::const_iterator it = grp.begin(); it != grp.end(); ++it) {
        App::DocumentObject *docObj = *it;
        if(docObj->getTypeId().isDerivedFrom(Drawing::FeatureViewCollection::getClassTypeId())) {
            Drawing::FeatureViewCollection *collection = dynamic_cast<Drawing::FeatureViewCollection *>(docObj);
            groupCount += collection->countChildren() + 1; // Include self
        } else {
            groupCount += 1;
        }
    }*/

    if(views.size() < grp.size()) {
        // An  view object has been added so update graphicsviews
        std::vector<App::DocumentObject*> notFnd;
        // Iterate through to find any views that are missing
        std::vector<QGraphicsItemView *>::const_iterator qview = views.begin();

        // TODO think of a better algorithm to deal with any changes to views list
        // Find any additions
        this->findMissingViews(grp, notFnd);

         // Iterate over missing views and add them
        for(std::vector<App::DocumentObject*>::const_iterator it = notFnd.begin(); it != notFnd.end(); ++it) {
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
             // we cannot guarantee  (*qview)->getViewObject() is safe
            fnd = this->orphanExists((*qview)->getViewName(), grp);

            if(fnd) {
                myViews.push_back(*qview);
            } else {
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
        if((*it)->getViewObject()->isTouched()) {
            (*it)->updateView();
        }
    }

}

void DrawingView::findMissingViews(const std::vector<App::DocumentObject*> &list, std::vector<App::DocumentObject*> &missing)
{
    for(std::vector<App::DocumentObject*>::const_iterator it = list.begin(); it != list.end(); ++it) {

        /*
        if((*it)->getTypeId().isDerivedFrom(Drawing::FeatureViewCollection::getClassTypeId())) {
            std::vector<App::DocumentObject*> missingChildViews;
            Drawing::FeatureViewCollection *collection = dynamic_cast<Drawing::FeatureViewCollection *>(*it);
            // Find Child Views recursively
            this->findMissingViews(collection->Views.getValues(), missingChildViews);

            // Append the views to current missing list
            for(std::vector<App::DocumentObject*>::const_iterator it = missingChildViews.begin(); it != missingChildViews.end(); ++it) {
                missing.push_back(*it);
            }
        }*/

        if(!this->hasQView(*it))
             missing.push_back(*it);
    }
}

/// Helper function
bool DrawingView::hasQView(App::DocumentObject *obj)
{
    const std::vector<QGraphicsItemView *> &views = m_view->getViews();
    std::vector<QGraphicsItemView *>::const_iterator qview = views.begin();

    while(qview != views.end()) {
        // Unsure if we can compare pointers so rely on name
        if(strcmp((*qview)->getViewObject()->getNameInDocument(), obj->getNameInDocument()) == 0) {
            return true;
        }
        qview++;
    }

    return false;
}

/// Helper function
bool DrawingView::orphanExists(const char *viewName, const std::vector<App::DocumentObject*> &list)
{
    for(std::vector<App::DocumentObject*>::const_iterator it = list.begin(); it != list.end(); ++it) {

        // Check child objects too recursively
        /*if((*it)->getTypeId().isDerivedFrom(Drawing::FeatureViewCollection::getClassTypeId())) {
            Drawing::FeatureViewCollection *collection = dynamic_cast<Drawing::FeatureViewCollection *>(*it);
            if(orphanExists(viewName, collection->Views.getValues()))
                return true;
        }*/

        // Unsure if we can compare pointers so rely on name
        if(strcmp(viewName, (*it)->getNameInDocument()) == 0) {
            return true;
        }
    }
    return false;
}

void DrawingView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;
    menu.addAction(this->m_backgroundAction);
    menu.addAction(this->m_outlineAction);
    menu.addAction(this->m_exportSVGAction);
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
//     printer.setOrientation(QPrinter::Landscape);
    QPrintDialog dlg(&printer, this);
    if (dlg.exec() == QDialog::Accepted) {
        print(&printer);
    }
}

void DrawingView::printPreview()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);


//     printer.setOrientation(QPrinter::Landscape);

    QPrintPreviewDialog dlg(&printer, this);
    connect(&dlg, SIGNAL(paintRequested (QPrinter *)),
            this, SLOT(print(QPrinter *)));
    dlg.exec();
}


// This SHOULD only be temporary
void DrawingView::saveSVG()
{

    Drawing::FeaturePage *page = pageGui->getPageObject();

    QSvgGenerator svgGen;

    QStringList filter;
    filter << QObject::tr("SVG(*.svg)");
    filter << QObject::tr("All Files (*.*)");

    QString fn = Gui::FileDialog::getSaveFileName(Gui::getMainWindow(), QObject::tr("Export page"), QString(), filter.join(QLatin1String(";;")));
    if (fn.isEmpty()) {
      Base::Console().Error("Cannot export SVG");
      return;
    }

    svgGen.setFileName(fn);
    svgGen.setSize(QSize((int) page->getPageWidth(), (int)page->getPageHeight()));
    svgGen.setViewBox(QRect(0, 0, page->getPageWidth(), page->getPageHeight()));

    bool block = this->blockConnection(true); // avoid to be notified by itself
    Gui::Selection().clearSelection();

    this->m_view->toggleEdit(false);
    this->m_view->scene()->update();

    Gui::Selection().clearSelection();
    QPainter p;

    p.begin(&svgGen);
    this->m_view->scene()->render(&p);
    p.end();
    // Reset
    this->m_view->toggleEdit(true);
}


void DrawingView::print(QPrinter* printer)
{
    Drawing::FeaturePage *page = pageGui->getPageObject();

//     if(strcmp(page->getPageOrientation(), "Landscape") == 0) {
//         printer->setOrientation(QPrinter::Landscape);
//     } else {
//         printer->setOrientation(QPrinter::Portrait);
//     }

    printer->setPaperSize(QSizeF(page->getPageWidth(), page->getPageHeight()), QPrinter::Millimeter);

    QPainter p(printer);

    bool block = this->blockConnection(true); // avoid to be notified by itself
    Gui::Selection().clearSelection();

    this->m_view->toggleEdit(false);

    Gui::Selection().clearSelection();
    p.begin(printer);

    this->m_view->scene()->render(&p);

    p.end();
    // Reset
    this->m_view->toggleEdit(true);
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
