/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Jürgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/

#include "PreCompiled.h"
#ifndef _PreComp_
# include <sstream>
# include <QCoreApplication>
# include <QDir>
# include <QFile>
# include <QFileInfo>
# include <QMessageBox>
# include <QRegExp>
#endif

#include <vector>

# include <App/DocumentObject.h>
# include <Gui/Action.h>
# include <Gui/Application.h>
# include <Gui/BitmapFactory.h>
# include <Gui/Command.h>
# include <Gui/Control.h>
# include <Gui/Document.h>
# include <Gui/Selection.h>
# include <Gui/MainWindow.h>
# include <Gui/FileDialog.h>
# include <Gui/ViewProvider.h>

# include <Mod/Part/App/PartFeature.h>

# include <Mod/Drawing/App/FeatureViewPart.h>
# include <Mod/Drawing/App/FeatureOrthoView.h>
# include <Mod/Drawing/App/FeatureViewOrthographic.h>
# include <Mod/Drawing/App/FeatureViewDimension.h>
# include <Mod/Drawing/App/FeaturePage.h>

# include "DrawingView.h"
# include "TaskDialog.h"
# include "TaskOrthoViews.h"
# include "ViewProviderPage.h"

using namespace DrawingGui;
using namespace std;

bool isDrawingPageActive(Gui::Document *doc)
{
    if (doc)
        // checks if a Sketch Viewprovider is in Edit and is in no special mode
        if (doc->getInEdit() && doc->getInEdit()->isDerivedFrom(DrawingGui::ViewProviderDrawingPage::getClassTypeId()))
            return true;
    return false;
}

//===========================================================================
// CmdDrawingOpen
//===========================================================================

DEF_STD_CMD(CmdDrawingOpen);

CmdDrawingOpen::CmdDrawingOpen()
  : Command("Drawing_Open")
{
    sAppModule      = "Drawing";
    sGroup          = QT_TR_NOOP("Drawing");
    sMenuText       = QT_TR_NOOP("Open SVG...");
    sToolTipText    = QT_TR_NOOP("Open a scalable vector graphic");
    sWhatsThis      = "Drawing_Open";
    sStatusTip      = sToolTipText;
    sPixmap         = "actions/document-new";
}


void CmdDrawingOpen::activated(int iMsg)
{
    // Reading an image
    QString filename = Gui::FileDialog::getOpenFileName(Gui::getMainWindow(), QObject::tr("Choose an SVG file to open"), QString::null,
                                           QObject::tr("Scalable Vector Graphics (*.svg *.svgz)"));
    if (!filename.isEmpty())
    {
        // load the file with the module
        Command::doCommand(Command::Gui, "import Drawing, DrawingGui");
        Command::doCommand(Command::Gui, "DrawingGui.open(\"%s\")", (const char*)filename.toUtf8());
    }
}

//===========================================================================
// Drawing_NewPage
//===========================================================================

DEF_STD_CMD_ACL(CmdDrawingNewPage);

CmdDrawingNewPage::CmdDrawingNewPage()
  : Command("Drawing_NewPage")
{
    sAppModule      = "Drawing";
    sGroup          = QT_TR_NOOP("Drawing");
    sMenuText       = QT_TR_NOOP("Insert new drawing");
    sToolTipText    = QT_TR_NOOP("Insert new drawing");
    sWhatsThis      = "Drawing_NewPage";
    sStatusTip      = sToolTipText;
}

void CmdDrawingNewPage::activated(int iMsg)
{
    Gui::ActionGroup* pcAction = qobject_cast<Gui::ActionGroup*>(_pcAction);
    QAction* a = pcAction->actions()[iMsg];

    std::string PageName = getUniqueObjectName("Page");
    std::string TemplateName = getUniqueObjectName("Template");

    QFileInfo tfi(a->property("Template").toString());
    if (tfi.isReadable()) {
        openCommand("Drawing create page");
        doCommand(Doc,"App.activeDocument().addObject('Drawing::FeaturePage','%s')",PageName.c_str());

        // Create the Template Object to attach to the page
        doCommand(Doc,"App.activeDocument().addObject('Drawing::FeatureSVGTemplate','%s')",TemplateName.c_str());
        doCommand(Doc,"App.activeDocument().%s.Template = App.activeDocument().%s",PageName.c_str(),TemplateName.c_str());

        commitCommand();
    }
    else {
        QMessageBox::critical(Gui::getMainWindow(),
            QLatin1String("No template"),
            QLatin1String("No template available for this page size"));
    }
}

Gui::Action * CmdDrawingNewPage::createAction(void)
{
    Gui::ActionGroup* pcAction = new Gui::ActionGroup(this, Gui::getMainWindow());
    pcAction->setDropDownMenu(true);
    applyCommandData(pcAction);

    QAction* defaultAction = 0;
    int defaultId = 0;

    std::string path = App::Application::getResourceDir();
    path += "Mod/Drawing/Templates/";
    QDir dir(QString::fromUtf8(path.c_str()), QString::fromAscii("*.svg"));
    for (unsigned int i=0; i<dir.count(); i++ ) {
        QRegExp rx(QString::fromAscii("(A|B|C|D|E)(\\d)_(Landscape|Portrait).svg"));
        if (rx.indexIn(dir[i]) > -1) {
            QString paper = rx.cap(1);
            int id = rx.cap(2).toInt();
            QString orientation = rx.cap(3);
            QFile file(QString::fromAscii(":/icons/actions/drawing-landscape-A0.svg"));
            QAction* a = pcAction->addAction(QString());
            if (file.open(QFile::ReadOnly)) {
                QString s = QString::fromAscii("style=\"font-size:22px\">%1%2</tspan></text>").arg(paper).arg(id);
                QByteArray data = file.readAll();
                data.replace("style=\"font-size:22px\">A0</tspan></text>", s.toAscii());
                a->setIcon(Gui::BitmapFactory().pixmapFromSvg(data, QSize(24,24)));
            }

            a->setProperty("TemplatePaper", paper);
            a->setProperty("TemplateOrientation", orientation);
            a->setProperty("TemplateId", id);
            a->setProperty("Template", dir.absoluteFilePath(dir[i]));

            if (id == 3) {
                defaultAction = a;
                defaultId = pcAction->actions().size() - 1;
            }
        }
    }

    _pcAction = pcAction;
    languageChange();
    if (defaultAction) {
        pcAction->setIcon(defaultAction->icon());
        pcAction->setProperty("defaultAction", QVariant(defaultId));
    }
    else if (!pcAction->actions().isEmpty()) {
        pcAction->setIcon(pcAction->actions()[0]->icon());
        pcAction->setProperty("defaultAction", QVariant(0));
    }

    return pcAction;
}

void CmdDrawingNewPage::languageChange()
{
    Command::languageChange();

    if (!_pcAction)
        return;
    Gui::ActionGroup* pcAction = qobject_cast<Gui::ActionGroup*>(_pcAction);
    QList<QAction*> a = pcAction->actions();
    for (QList<QAction*>::iterator it = a.begin(); it != a.end(); ++it) {
        QString paper = (*it)->property("TemplatePaper").toString();
        int id = (*it)->property("TemplateId").toInt();
        QString orientation = (*it)->property("TemplateOrientation").toString();
        if (orientation.compare(QLatin1String("landscape"), Qt::CaseInsensitive) == 0)
            orientation = QCoreApplication::translate("Drawing_NewPage", "Landscape", 0, QCoreApplication::CodecForTr);
        else if (orientation.compare(QLatin1String("portrait"), Qt::CaseInsensitive) == 0)
            orientation = QCoreApplication::translate("Drawing_NewPage", "Portrait", 0, QCoreApplication::CodecForTr);

        (*it)->setText(QCoreApplication::translate(
            "Drawing_NewPage", "%1%2 %3", 0,
            QCoreApplication::CodecForTr)
            .arg(paper)
            .arg(id)
            .arg(orientation));
        (*it)->setToolTip(QCoreApplication::translate(
            "Drawing_NewPage", "Insert new %1%2 %3 drawing", 0,
            QCoreApplication::CodecForTr)
            .arg(paper)
            .arg(id)
            .arg(orientation));
    }
}

bool CmdDrawingNewPage::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}

//===========================================================================
// Drawing_NewA3Landscape
//===========================================================================

DEF_STD_CMD_A(CmdDrawingNewA3Landscape);

CmdDrawingNewA3Landscape::CmdDrawingNewA3Landscape()
  : Command("Drawing_NewA3Landscape")
{
    sAppModule      = "Drawing";
    sGroup          = QT_TR_NOOP("Drawing");
    sMenuText       = QT_TR_NOOP("Insert new A3 landscape drawing");
    sToolTipText    = QT_TR_NOOP("Insert new A3 landscape drawing");
    sWhatsThis      = "Drawing_NewA3Landscape";
    sStatusTip      = sToolTipText;
    sPixmap         = "actions/drawing-landscape-A3";
}

void CmdDrawingNewA3Landscape::activated(int iMsg)
{
    std::string FeatName = getUniqueObjectName("Page");

    openCommand("Drawing create page");
    doCommand(Doc,"App.activeDocument().addObject('Drawing::FeaturePage','%s')",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Template = 'A3_Landscape.svg'",FeatName.c_str());
    commitCommand();
}

bool CmdDrawingNewA3Landscape::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}


//===========================================================================
// Drawing_NewView
//===========================================================================

DEF_STD_CMD(CmdDrawingNewView);

CmdDrawingNewView::CmdDrawingNewView()
  : Command("Drawing_NewView")
{
    sAppModule      = "Drawing";
    sGroup          = QT_TR_NOOP("Drawing");
    sMenuText       = QT_TR_NOOP("Insert view in drawing");
    sToolTipText    = QT_TR_NOOP("Insert a new View of a Part in the active drawing");
    sWhatsThis      = "Drawing_NewView";
    sStatusTip      = sToolTipText;
    sPixmap         = "actions/drawing-view";
}

void CmdDrawingNewView::activated(int iMsg)
{
    std::vector<App::DocumentObject*> shapes = getSelection().getObjectsOfType(Part::Feature::getClassTypeId());
    if (shapes.empty()) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select a Part object."));
        return;
    }

    std::vector<App::DocumentObject*> pages = this->getDocument()->getObjectsOfType(Drawing::FeaturePage::getClassTypeId());
    if (pages.empty()){
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("No page to insert"),
            QObject::tr("Create a page to insert."));
        return;
    }

    std::string PageName = pages.front()->getNameInDocument();

    openCommand("Create view");
    for (std::vector<App::DocumentObject*>::iterator it = shapes.begin(); it != shapes.end(); ++it) {
        std::string FeatName = getUniqueObjectName("View");
        doCommand(Doc,"App.activeDocument().addObject('Drawing::FeatureViewPart','%s')",FeatName.c_str());
        doCommand(Doc,"App.activeDocument().%s.Source = App.activeDocument().%s",FeatName.c_str(),(*it)->getNameInDocument());
//         doCommand(Doc,"App.activeDocument().%s.Direction = (0.0,0.0,1.0)",FeatName.c_str());
        doCommand(Doc,"App.activeDocument().%s.X = 0.0",FeatName.c_str());
        doCommand(Doc,"App.activeDocument().%s.Y = 0.0",FeatName.c_str());
        doCommand(Doc,"App.activeDocument().%s.Scale = 1.0",FeatName.c_str());
//         doCommand(Doc,"App.activeDocument().%s.addObject(App.activeDocument().%s)",PageName.c_str(),);
        Drawing::FeaturePage *page = dynamic_cast<Drawing::FeaturePage *>(pages.front());
        page->addView(page->getDocument()->getObject(FeatName.c_str()));
    }
    updateActive();
    commitCommand();
}


//===========================================================================
// Drawing_NewDimension
//===========================================================================

DEF_STD_CMD(CmdDrawingNewDimension);

CmdDrawingNewDimension::CmdDrawingNewDimension()
  : Command("Drawing_NewDimension")
{
    sAppModule      = "Drawing";
    sGroup          = QT_TR_NOOP("Drawing");
    sMenuText       = QT_TR_NOOP("Insert a dimension into the drawing");
    sToolTipText    = QT_TR_NOOP("Insert a new dimension feature into the drawing object");
    sWhatsThis      = "Drawing_NewDimension";
    sStatusTip      = sToolTipText;
    sPixmap         = "actions/drawing-annotation";
}

void CmdDrawingNewDimension::activated(int iMsg)
{

      // get the selection
    std::vector<Gui::SelectionObject> selection = getSelection().getSelectionEx();
    if(selection.size() == 0 || !selection[0].getObject()){
        return;
    }

    Drawing::FeatureViewPart * Obj = dynamic_cast<Drawing::FeatureViewPart *>(selection[0].getObject());

    if(!Obj) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong sized selection"),
                             QObject::tr("Incorrect selection"));
                             return;
    }

    App::DocumentObject *docObj = Obj->Source.getValue();

    // get the needed lists and objects
    const std::vector<std::string> &SubNames = selection[0].getSubNames();

    if (SubNames.size() != 1 && SubNames.size() != 2){
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong sized selection"),
            QObject::tr("Incorrect selection"));
        return;
    }

    std::vector<App::DocumentObject*> pages = this->getDocument()->getObjectsOfType(Drawing::FeaturePage::getClassTypeId());
    if (pages.empty()){
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("No page to insert"),
            QObject::tr("Create a page to insert."));
        return;
    }
    Drawing::FeatureViewDimension *dim = 0;
    std::string support = selection[0].getAsPropertyLinkSubString();
    std::string FeatName = getUniqueObjectName("Dimension");

    std::string dimType;

    openCommand("Create Dimension");
    doCommand(Doc,"App.activeDocument().addObject('Drawing::FeatureViewDimension','%s')",FeatName.c_str());

    if (SubNames.size() == 1) {
        // Selected edge constraint
        if (SubNames[0].size() > 4 && SubNames[0].substr(0,4) == "Edge") {
            int GeoId = std::atoi(SubNames[0].substr(4,4000).c_str());

            DrawingGeometry::BaseGeom *geom = Obj->getCompleteEdge(GeoId);

            dimType = "Distance";

            if(geom->geomType == DrawingGeometry::CIRCLE ||
               geom->geomType == DrawingGeometry::ARCOFCIRCLE ||
               geom->geomType == DrawingGeometry::ELLIPSE ||
               geom->geomType == DrawingGeometry::ARCOFELLIPSE ) {
                // Radius Constraint
                dimType = "Radius";
            }

            doCommand(Doc,"App.activeDocument().%s.Type = '%s'",FeatName.c_str()
                                                               ,dimType.c_str());

            dim = dynamic_cast<Drawing::FeatureViewDimension *>(this->getDocument()->getObject(FeatName.c_str()));
            dim->References.setValue(Obj, SubNames[0].c_str());
        } else {
            return;
        }

    } else if(SubNames.size() == 2) {
        if (SubNames[0].size() > 6 && SubNames[0].substr(0,6) == "Vertex" &&
            SubNames[1].size() > 6 && SubNames[1].substr(0,6) == "Vertex") {
            int GeoId1 = std::atoi(SubNames[0].substr(6,4000).c_str());
            int GeoId2 = std::atoi(SubNames[1].substr(6,4000).c_str());

            dimType = "Distance";
            doCommand(Doc,"App.activeDocument().%s.Type = '%s'",FeatName.c_str()
                                                               ,dimType.c_str());

            dim = dynamic_cast<Drawing::FeatureViewDimension *>(this->getDocument()->getObject(FeatName.c_str()));
            std::vector<App::DocumentObject *> objs;
            objs.push_back(Obj);
            objs.push_back(Obj);
            std::vector<std::string> subs;
            subs.push_back(SubNames[0]);
            subs.push_back(SubNames[1]);
            dim->References.setValues(objs, subs);

        } else if(SubNames[0].size() > 4 && SubNames[0].substr(0,4) == "Edge" &&
                  SubNames[1].size() > 4 && SubNames[1].substr(0,4) == "Edge") {
            int GeoId1 = std::atoi(SubNames[0].substr(4,4000).c_str());
            int GeoId2 = std::atoi(SubNames[1].substr(4,4000).c_str());

            // Project the edges
            Drawing::FeatureViewPart *viewPart = dynamic_cast<Drawing::FeatureViewPart * >(Obj);
            DrawingGeometry::BaseGeom *ed1 = viewPart->getCompleteEdge(GeoId1);
            DrawingGeometry::BaseGeom *ed2 = viewPart->getCompleteEdge(GeoId2);

            if(ed1->geomType == DrawingGeometry::GENERIC &&
               ed2->geomType == DrawingGeometry::GENERIC) {
                DrawingGeometry::Generic *gen1 = static_cast<DrawingGeometry::Generic *>(ed1);
                DrawingGeometry::Generic *gen2 = static_cast<DrawingGeometry::Generic *>(ed2);
                if(gen1->points.size() > 2 || gen2->points.size() > 2) {
                    // Only support straight line edges
                    QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Please select only straight line edges"),
                                                               QObject::tr("Incorrect selection"));
                    return;
                }

                // Construct edges
                Base::Vector2D lin1 = gen1->points.at(1) - gen1->points.at(0);
                Base::Vector2D lin2 = gen2->points.at(1) - gen2->points.at(0);

                // Cross product
                double xprod = fabs(lin1.fX * lin2.fY - lin1.fY * lin2.fX);

                Base::Console().Log("xprod: %f\n", xprod);
                if(xprod < FLT_EPSILON) {
                    if(fabs(lin1.fX) < FLT_EPSILON && fabs(lin2.fX) < FLT_EPSILON)
                        dimType = "DistanceX";
                    else if(fabs(lin1.fY) < FLT_EPSILON && fabs(lin2.fY) < FLT_EPSILON)
                        dimType = "DistanceY";
                    else
                        dimType = "Distance";
                } else {
                    // Angle Measurement
                    dimType = "Angle";
                }

            } else {
                // Only support straight line edges
                QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Please provide a valid selection: Only straight line edges can be currently used"),
                                                           QObject::tr("Incorrect selection"));
                return;
            }

            doCommand(Doc,"App.activeDocument().%s.Type = '%s'",FeatName.c_str()
                                                               ,dimType.c_str());

            dim = dynamic_cast<Drawing::FeatureViewDimension *>(this->getDocument()->getObject(FeatName.c_str()));
            std::vector<App::DocumentObject *> objs;
            objs.push_back(Obj);
            objs.push_back(Obj);
            std::vector<std::string> subs;
            subs.push_back(SubNames[0]);
            subs.push_back(SubNames[1]);
            dim->References.setValues(objs, subs);

        } else {

            QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Please provide a valid selection"),
            QObject::tr("Incorrect selection"));
            return;
        }
    }

    QString contentStr = QString::fromAscii("%value");

    if(strcmp(dimType.c_str(), "Angle") == 0) {
        // Append the degree symbol sign using unicode to content string
        contentStr += QString(QChar(0x00b0));

    } else if(strcmp(dimType.c_str(), "Radius") == 0) {
        contentStr.prepend(QString::fromAscii("r"));
    } else if(strcmp(dimType.c_str(), "Diameter") == 0) {
        contentStr += QString(QChar(0x00d8));
    }


    doCommand(Doc,"App.activeDocument().%s.Content = '%s'",FeatName.c_str()
                                                          ,contentStr.toStdString().c_str());

    // Check if the part is an orthographic view;
    Drawing::FeatureOrthoView *orthoView = dynamic_cast<Drawing::FeatureOrthoView *>(Obj);
    if(orthoView) {
        // Set the dimension to projected type
        doCommand(Doc,"App.activeDocument().%s.ProjectionType = 'Projected'",FeatName.c_str());
        dim->ProjectionType.StatusBits.set(2); // Set the projection type to read only
    }

    dim->execute();
//     App::DocumentObject *dimObj = this->getDocument()->addObject("Drawing::FeatureViewDimension", getUniqueObjectName("Dimension").c_str());
//     Drawing::FeatureViewDimension *dim = dynamic_cast<Drawing::FeatureViewDimension *>(dimObj);

//     doCommand(Doc,"App.activeDocument().%s.References = %s",FeatName.c_str(), support.c_str());


    Drawing::FeaturePage *page = dynamic_cast<Drawing::FeaturePage *>(pages.front());
    page->addView(page->getDocument()->getObject(FeatName.c_str()));

    updateActive();
    commitCommand();
}

//===========================================================================
// Drawing_NewViewSection
//===========================================================================

DEF_STD_CMD(CmdDrawingNewViewSection);

CmdDrawingNewViewSection::CmdDrawingNewViewSection()
  : Command("Drawing_NewViewSection")
{
    sAppModule      = "Drawing";
    sGroup          = QT_TR_NOOP("Drawing");
    sMenuText       = QT_TR_NOOP("Insert view in drawing");
    sToolTipText    = QT_TR_NOOP("Insert a new View of a Part in the active drawing");
    sWhatsThis      = "Drawing_NewViewSecton";
    sStatusTip      = sToolTipText;
    sPixmap         = "actions/drawing-view";
}

void CmdDrawingNewViewSection::activated(int iMsg)
{
    std::vector<App::DocumentObject*> shapes = getSelection().getObjectsOfType(Part::Feature::getClassTypeId());
    if (shapes.empty()) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select a Part object."));
        return;
    }

    std::vector<App::DocumentObject*> pages = this->getDocument()->getObjectsOfType(Drawing::FeaturePage::getClassTypeId());
    if (pages.empty()){
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("No page to insert"),
            QObject::tr("Create a page to insert."));
        return;
    }

    std::string PageName = pages.front()->getNameInDocument();

    openCommand("Create view");
    for (std::vector<App::DocumentObject*>::iterator it = shapes.begin(); it != shapes.end(); ++it) {
        std::string FeatName = getUniqueObjectName("View");
        doCommand(Doc,"App.activeDocument().addObject('Drawing::FeatureViewSection','%s')",FeatName.c_str());
        doCommand(Doc,"App.activeDocument().%s.Source = App.activeDocument().%s",FeatName.c_str(),(*it)->getNameInDocument());
        doCommand(Doc,"App.activeDocument().%s.Direction = (0.0,0.0,1.0)",FeatName.c_str());
        doCommand(Doc,"App.activeDocument().%s.X = 10.0",FeatName.c_str());
        doCommand(Doc,"App.activeDocument().%s.Y = 10.0",FeatName.c_str());
        doCommand(Doc,"App.activeDocument().%s.Scale = 1.0",FeatName.c_str());
//         doCommand(Doc,"App.activeDocument().%s.addObject(App.activeDocument().%s)",PageName.c_str(),);
        Drawing::FeaturePage *page = dynamic_cast<Drawing::FeaturePage *>(pages.front());
        page->addView(page->getDocument()->getObject(FeatName.c_str()));
    }
    updateActive();
    commitCommand();
}

//===========================================================================
// Drawing_OrthoView
//===========================================================================

DEF_STD_CMD(CmdDrawingOrthoViews);

CmdDrawingOrthoViews::CmdDrawingOrthoViews()
  : Command("Drawing_OrthoViews")
{
    sAppModule      = "Drawing";
    sGroup          = QT_TR_NOOP("Drawing");
    sMenuText       = QT_TR_NOOP("Insert orthographic views");
    sToolTipText    = QT_TR_NOOP("Insert an orthographic projection of a part in the active drawing");
    sWhatsThis      = "Drawing_OrthoView";
    sStatusTip      = sToolTipText;
    sPixmap         = "actions/drawing-orthoviews";
}

void CmdDrawingOrthoViews::activated(int iMsg)
{
    std::vector<App::DocumentObject*> shapes = getSelection().getObjectsOfType(Part::Feature::getClassTypeId());
    if (shapes.size() != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select a Part object."));
        return;
    }

    std::vector<App::DocumentObject*> pages = this->getDocument()->getObjectsOfType(Drawing::FeaturePage::getClassTypeId());
    if (pages.empty()){
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("No page to insert"),
            QObject::tr("Create a page to insert views into."));
        return;
    }

    std::string PageName = pages.front()->getNameInDocument();

    openCommand("Create Orthographic View");
    for (std::vector<App::DocumentObject*>::iterator it = shapes.begin(); it != shapes.end(); ++it) {
        std::string FeatName = getUniqueObjectName("cView");
        doCommand(Doc,"App.activeDocument().addObject('Drawing::FeatureViewOrthographic','%s')",FeatName.c_str());
        doCommand(Doc,"App.activeDocument().%s.Source = App.activeDocument().%s",FeatName.c_str(),(*it)->getNameInDocument());
        doCommand(Doc,"App.activeDocument().%s.X = 0.0",    FeatName.c_str());
        doCommand(Doc,"App.activeDocument().%s.Y = 0.0",    FeatName.c_str());
        doCommand(Doc,"App.activeDocument().%s.Scale = 1.0",FeatName.c_str());
//         doCommand(Doc,"App.activeDocument().%s.addObject(App.activeDocument().%s)",PageName.c_str(),);

        App::DocumentObject *docObj = getDocument()->getObject(FeatName.c_str());
        Drawing::FeatureViewOrthographic *viewOrtho = dynamic_cast<Drawing::FeatureViewOrthographic *>(docObj);

        viewOrtho->addView("Front");
        viewOrtho->addView("Top");
        viewOrtho->addView("Left");
        
        Drawing::FeaturePage *page = dynamic_cast<Drawing::FeaturePage *>(pages.front());
        page->addView(page->getDocument()->getObject(FeatName.c_str()));
    }
    updateActive();
    commitCommand();

    Gui::Control().showDialog(new TaskDlgOrthoViews());
}


//===========================================================================
// Drawing_OpenBrowserView
//===========================================================================

DEF_STD_CMD_A(CmdDrawingOpenBrowserView);

CmdDrawingOpenBrowserView::CmdDrawingOpenBrowserView()
  : Command("Drawing_OpenBrowserView")
{
    // seting the
    sGroup        = QT_TR_NOOP("Drawing");
    sMenuText     = QT_TR_NOOP("Open &browser view");
    sToolTipText  = QT_TR_NOOP("Opens the selected page in a browser view");
    sWhatsThis    = "Drawing_OpenBrowserView";
    sStatusTip    = QT_TR_NOOP("Opens the selected page in a browser view");
    sPixmap       = "actions/drawing-openbrowser";
}

void CmdDrawingOpenBrowserView::activated(int iMsg)
{
    unsigned int n = getSelection().countObjectsOfType(Drawing::FeaturePage::getClassTypeId());
    if (n != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Wrong selection"),
            QObject::tr("Select one Page object."));
        return;
    }
    std::vector<Gui::SelectionSingleton::SelObj> Sel = getSelection().getSelection();
    doCommand(Doc,"PageName = App.activeDocument().%s.PageResult",Sel[0].FeatName);
    doCommand(Doc,"import WebGui");
    doCommand(Doc,"WebGui.openBrowser(PageName)");
}

bool CmdDrawingOpenBrowserView::isActive(void)
{
    return (getActiveGuiDocument() ? true : false);
}

//===========================================================================
// Drawing_Annotation
//===========================================================================

DEF_STD_CMD_A(CmdDrawingAnnotation);

CmdDrawingAnnotation::CmdDrawingAnnotation()
  : Command("Drawing_Annotation")
{
    // seting the
    sGroup        = QT_TR_NOOP("Drawing");
    sMenuText     = QT_TR_NOOP("&Annotation");
    sToolTipText  = QT_TR_NOOP("Inserts an Annotation view in the active drawing");
    sWhatsThis    = "Drawing_Annotation";
    sStatusTip    = QT_TR_NOOP("Inserts an Annotation view in the active drawing");
    sPixmap       = "actions/drawing-annotation";
}

void CmdDrawingAnnotation::activated(int iMsg)
{

    std::vector<App::DocumentObject*> pages = this->getDocument()->getObjectsOfType(Drawing::FeaturePage::getClassTypeId());
    if (pages.empty()){
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("No page to insert"),
            QObject::tr("Create a page to insert."));
        return;
    }
    std::string PageName = pages.front()->getNameInDocument();
    std::string FeatName = getUniqueObjectName("Annotation");
    openCommand("Create Annotation");
    doCommand(Doc,"App.activeDocument().addObject('Drawing::FeatureViewAnnotation','%s')",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.X = 10.0",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Y = 10.0",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Scale = 7.0",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.addObject(App.activeDocument().%s)",PageName.c_str(),FeatName.c_str());
    updateActive();
    commitCommand();
}

bool CmdDrawingAnnotation::isActive(void)
{
    return (getActiveGuiDocument() ? true : false);
}


//===========================================================================
// Drawing_Clip
//===========================================================================

DEF_STD_CMD_A(CmdDrawingClip);

CmdDrawingClip::CmdDrawingClip()
  : Command("Drawing_Clip")
{
    // seting the
    sGroup        = QT_TR_NOOP("Drawing");
    sMenuText     = QT_TR_NOOP("&Clip");
    sToolTipText  = QT_TR_NOOP("Inserts a clip group in the active drawing");
    sWhatsThis    = "Drawing_Annotation";
    sStatusTip    = QT_TR_NOOP("Inserts a clip group in the active drawing");
    sPixmap       = "actions/drawing-clip";
}

void CmdDrawingClip::activated(int iMsg)
{

    std::vector<App::DocumentObject*> pages = this->getDocument()->getObjectsOfType(Drawing::FeaturePage::getClassTypeId());
    if (pages.empty()){
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("No page to insert"),
            QObject::tr("Create a page to insert."));
        return;
    }
    std::string PageName = pages.front()->getNameInDocument();
    std::string FeatName = getUniqueObjectName("Clip");
    openCommand("Create Clip");
    doCommand(Doc,"App.activeDocument().addObject('Drawing::FeatureClip','%s')",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.addObject(App.activeDocument().%s)",PageName.c_str(),FeatName.c_str());
    updateActive();
    commitCommand();
}

bool CmdDrawingClip::isActive(void)
{
    return (getActiveGuiDocument() ? true : false);
}


//===========================================================================
// Drawing_Symbol
//===========================================================================

DEF_STD_CMD_A(CmdDrawingSymbol);

CmdDrawingSymbol::CmdDrawingSymbol()
  : Command("Drawing_Symbol")
{
    // seting the
    sGroup        = QT_TR_NOOP("Drawing");
    sMenuText     = QT_TR_NOOP("&Symbol");
    sToolTipText  = QT_TR_NOOP("Inserts a symbol from a svg file in the active drawing");
    sWhatsThis    = "Drawing_Symbol";
    sStatusTip    = QT_TR_NOOP("Inserts a symbol from a svg file in the active drawing");
    sPixmap       = "actions/drawing-symbol";
}

void CmdDrawingSymbol::activated(int iMsg)
{

    std::vector<App::DocumentObject*> pages = this->getDocument()->getObjectsOfType(Drawing::FeaturePage::getClassTypeId());
    if (pages.empty()){
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("No page to insert"),
            QObject::tr("Create a page to insert."));
        return;
    }
    // Reading an image
    QString filename = Gui::FileDialog::getOpenFileName(Gui::getMainWindow(), QObject::tr("Choose an SVG file to open"), QString::null,
                                           QObject::tr("Scalable Vector Graphics (*.svg *.svgz)"));
    if (!filename.isEmpty())
    {
        std::string PageName = pages.front()->getNameInDocument();
        std::string FeatName = getUniqueObjectName("Symbol");
        openCommand("Create Symbol");
        doCommand(Doc,"import Drawing");
        doCommand(Doc,"f = open(\"%s\",\"r\")",(const char*)filename.toUtf8());
        doCommand(Doc,"svg = f.read()");
        doCommand(Doc,"f.close()");
        doCommand(Doc,"App.activeDocument().addObject('Drawing::FeatureViewSymbol','%s')",FeatName.c_str());
        doCommand(Doc,"App.activeDocument().%s.Symbol = Drawing.removeSvgTags(svg)",FeatName.c_str());
        doCommand(Doc,"App.activeDocument().%s.addObject(App.activeDocument().%s)",PageName.c_str(),FeatName.c_str());
        updateActive();
        commitCommand();
    }
}

bool CmdDrawingSymbol::isActive(void)
{
    return (getActiveGuiDocument() ? true : false);
}


//===========================================================================
// Drawing_ExportPage
//===========================================================================

DEF_STD_CMD_A(CmdDrawingExportPage);

CmdDrawingExportPage::CmdDrawingExportPage()
  : Command("Drawing_ExportPage")
{
    // seting the
    sGroup        = QT_TR_NOOP("File");
    sMenuText     = QT_TR_NOOP("&Export page...");
    sToolTipText  = QT_TR_NOOP("Export a page to an SVG file");
    sWhatsThis    = "Drawing_ExportPage";
    sStatusTip    = QT_TR_NOOP("Export a page to an SVG file");
    sPixmap       = "document-save";
}

void CmdDrawingExportPage::activated(int iMsg)
{
    Base::Console().Error("Need to implemenet");
}

bool CmdDrawingExportPage::isActive(void)
{
    return (getActiveGuiDocument() ? true : false);
}

//===========================================================================
// Drawing_ProjectShape
//===========================================================================

DEF_STD_CMD_A(CmdDrawingProjectShape);

CmdDrawingProjectShape::CmdDrawingProjectShape()
  : Command("Drawing_ProjectShape")
{
    // seting the
    sGroup        = QT_TR_NOOP("Drawing");
    sMenuText     = QT_TR_NOOP("Project shape...");
    sToolTipText  = QT_TR_NOOP("Project shape onto a user-defined plane");
    sStatusTip    = QT_TR_NOOP("Project shape onto a user-defined plane");
    sWhatsThis    = "Drawing_ProjectShape";
}

void CmdDrawingProjectShape::activated(int iMsg)
{
    Gui::TaskView::TaskDialog* dlg = Gui::Control().activeDialog();
    if (!dlg) {
        dlg = new DrawingGui::TaskProjection();
        dlg->setButtonPosition(Gui::TaskView::TaskDialog::South);
    }
    Gui::Control().showDialog(dlg);
}

bool CmdDrawingProjectShape::isActive(void)
{
    int ct = Gui::Selection().countObjectsOfType(Part::Feature::getClassTypeId());
    return (ct > 0 && !Gui::Control().activeDialog());
}



void CreateDrawingCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

    rcCmdMgr.addCommand(new CmdDrawingOpen());
    rcCmdMgr.addCommand(new CmdDrawingNewPage());
    rcCmdMgr.addCommand(new CmdDrawingNewA3Landscape());
    rcCmdMgr.addCommand(new CmdDrawingNewView());
    rcCmdMgr.addCommand(new CmdDrawingNewDimension());
    rcCmdMgr.addCommand(new CmdDrawingNewViewSection());
    rcCmdMgr.addCommand(new CmdDrawingOrthoViews());
    rcCmdMgr.addCommand(new CmdDrawingOpenBrowserView());
    rcCmdMgr.addCommand(new CmdDrawingAnnotation());
    rcCmdMgr.addCommand(new CmdDrawingClip());
    rcCmdMgr.addCommand(new CmdDrawingSymbol());
    rcCmdMgr.addCommand(new CmdDrawingExportPage());
    rcCmdMgr.addCommand(new CmdDrawingProjectShape());
}
