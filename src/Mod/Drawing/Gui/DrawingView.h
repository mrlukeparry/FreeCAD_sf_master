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

#ifndef DRAWINGGUI_DRAWINGVIEW_H
#define DRAWINGGUI_DRAWINGVIEW_H

#include <Gui/MDIView.h>
#include <Gui/Selection.h>
#include <QGraphicsView>

#include <App/PropertyLinks.h>

QT_BEGIN_NAMESPACE
class QSlider;
class QAction;
class QActionGroup;
class QFile;
class QPopupMenu;
class QToolBar;
class QSvgWidget;
class QScrollArea;
class QPrinter;
QT_END_NAMESPACE

namespace Drawing {
class FeaturePage;
}

namespace DrawingGui
{

class CanvasView;

class DrawingGuiExport DrawingView : public Gui::MDIView, public Gui::SelectionObserver
{
    Q_OBJECT

public:
    DrawingView(Gui::Document* doc, QWidget* parent = 0);
    ~DrawingView();

public Q_SLOTS:
    void attachPageObject(Drawing::FeaturePage *pageFeature);
    void setRenderer(QAction *action);
    void viewAll();
    void selectionChanged();
    void preSelectionChanged(const QPoint &pos);
    void updateDrawing();

public:
   /// Observer message from the Selection
    void onSelectionChanged(const Gui::SelectionChanges& msg);

    bool onMsg(const char* pMsg,const char** ppReturn);
    bool onHasMsg(const char* pMsg) const;
    void print();
    void printPdf();
    void printPreview();
    void print(QPrinter* printer);
    PyObject* getPyObject();

protected:
    App::PropertyLink pageFeat;

protected:
    void attachView(App::DocumentObject *obj);
    void contextMenuEvent(QContextMenuEvent *event);

private:
    QAction *m_nativeAction;
    QAction *m_glAction;
    QAction *m_imageAction;
    QAction *m_highQualityAntialiasingAction;
    QAction *m_backgroundAction;
    QAction *m_outlineAction;

    CanvasView *m_view;

    QString m_currentPath;
};

} // namespace DrawingViewGui

#endif // DRAWINGGUI_DRAWINGVIEW_H
