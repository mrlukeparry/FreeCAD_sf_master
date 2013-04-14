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
  #include <BRep_Builder.hxx>
  #include <TopoDS_Compound.hxx>
  # include <TopoDS_Shape.hxx>
  # include <TopoDS_Edge.hxx>
  # include <TopoDS.hxx>
  # include <BRepAdaptor_Curve.hxx>
  
  # include <QAction>
  # include <QApplication>
  # include <QContextMenuEvent>
  # include <QGraphicsScene>
  # include <QGridLayout>
  # include <QMenu>
  # include <QMessageBox>
  # include <QMouseEvent>

  # include <QPainterPathStroker>
  # include <QPainter>
  # include <strstream>
  # include <math.h>
  # include <QGraphicsPathItem>
  # include <QGraphicsTextItem>
#endif

# include <Base/Console.h>
# include <Base/Exception.h>
# include <Gui/Command.h>

# include <Mod/Part/App/PartFeature.h>

# include "../App/FeatureViewDimension.h"
# include "../App/FeatureViewPart.h"
# include "QGraphicsItemViewDimension.h"

using namespace DrawingGui;


QGraphicsItemDatumLabel::QGraphicsItemDatumLabel(int ref, QGraphicsScene *scene  ) : reference(ref) 
{
    if(scene) {
        scene->addItem(this);
    }
    this->posX = 0;
    this->posY = 0;
    this->setFlag(ItemSendsGeometryChanges, true);
    this->setFlag(ItemIsMovable, true);
    this->setFlag(ItemIsSelectable, true);
    this->setAcceptHoverEvents(true);    
}

QVariant QGraphicsItemDatumLabel::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedHasChanged && scene()) {
        // value is the new position.
        if(isSelected()) {
          this->setDefaultTextColor(Qt::blue); 
        } else {
          this->setDefaultTextColor(Qt::black);
        }
        update();
    } else if(change == ItemPositionHasChanged && scene()) {
        updatePos();
        Q_EMIT dragging();
    }
    
    return QGraphicsItem::itemChange(change, value);
}
void QGraphicsItemDatumLabel::updatePos()
{
    this->posX = this->x() + this->boundingRect().width() / 2;
    this->posY = this->y() + this->boundingRect().height() / 2;
}

void QGraphicsItemDatumLabel::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    this->setDefaultTextColor(Qt::blue);
    update();
}

void QGraphicsItemDatumLabel::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{    
    QGraphicsItemView *view = dynamic_cast<QGraphicsItemView *> (this->parentItem());
    assert(view != 0);
    
    if(!isSelected() && !view->isSelected()) {
        this->setDefaultTextColor(Qt::black);
        update();
    }
}

void QGraphicsItemDatumLabel::mouseReleaseEvent( QGraphicsSceneMouseEvent * event)
{       
    if(scene() && this == scene()->mouseGrabberItem()) {
        Q_EMIT dragFinished();
    }
    QGraphicsItem::mouseReleaseEvent(event);   
}

QGraphicsItemViewDimension::QGraphicsItemViewDimension(const QPoint &pos, QGraphicsScene *scene) :QGraphicsItemView(pos, scene)                 
{
    setHandlesChildEvents(false);
    this->setFlag(QGraphicsItem::ItemIsMovable, false);
    
    QGraphicsItemDatumLabel *dLabel = new QGraphicsItemDatumLabel();
    QGraphicsPathItem *arrws = new QGraphicsPathItem();
   
         // connecting the needed signals
    QObject::connect(
        dLabel  , SIGNAL(dragging()),
        this    , SLOT  (datumLabelDragged()));
    
    QObject::connect(
        dLabel  , SIGNAL(dragFinished()),
        this    , SLOT  (datumLabelDragFinished()));
    
    this->arrows  = arrws;    
    this->datumLabel = dLabel;
    
    this->addToGroup(arrows);
    this->addToGroup(datumLabel);
}

QGraphicsItemViewDimension::~QGraphicsItemViewDimension()
{
    
}

void QGraphicsItemViewDimension::updateDim()
{
    // For now assume only show absolute dimension values
    bool absolute = true;

    if(this->viewObject == 0 || !this->viewObject->isDerivedFrom(Drawing::FeatureViewDimension::getClassTypeId()))
        return;
        
    Drawing::FeatureViewDimension *dim = dynamic_cast<Drawing::FeatureViewDimension *>(this->viewObject);
    
    QString str;

    int precision = dim->Precision.getValue();
    str.setNum((absolute) ? abs(dim->getValue()): dim->getValue(), 'g', precision);
    
    QGraphicsItemDatumLabel *dLabel = dynamic_cast<QGraphicsItemDatumLabel *>(this->datumLabel);
     
    QFont font = dLabel->font();
    font.setPointSizeF(dim->Fontsize.getValue());
    font.setFamily(QString::fromAscii(dim->Font.getValue()));
    
    dLabel->setPlainText(str);
    dLabel->setFont(font);    
    dLabel->updatePos();
}

void QGraphicsItemViewDimension::datumLabelDragged()
{
//     int x = this->datumLabel->posX;
//     int y = this->datumLabel->posY;

    draw();
}

void QGraphicsItemViewDimension::datumLabelDragFinished()
{
//     int x = this->datumLabel->posX;
//     int y = this->datumLabel->posY;

    if(this->viewObject == 0 || !this->viewObject->isDerivedFrom(Drawing::FeatureViewDimension::getClassTypeId()))
        return;
        
    Drawing::FeatureViewDimension *dim = dynamic_cast<Drawing::FeatureViewDimension *>(this->viewObject);
    QGraphicsItemDatumLabel *datumLbl = dynamic_cast<QGraphicsItemDatumLabel *>(this->datumLabel);

    Gui::Command::openCommand("Drag Dimension");
    Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.X = %f", dim->getNameInDocument(), datumLbl->X());
    Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.Y = %f", dim->getNameInDocument(), datumLbl->Y());
    Gui::Command::commitCommand();
    Gui::Command::updateActive();
}


void QGraphicsItemViewDimension::draw()
{        
    // Iterate
    if(this->viewObject == 0 || !this->viewObject->isDerivedFrom(Drawing::FeatureViewDimension::getClassTypeId()))
        return;
        
    Drawing::FeatureViewDimension *dim = dynamic_cast<Drawing::FeatureViewDimension *>(this->viewObject);
    QGraphicsItemDatumLabel *lbl = dynamic_cast<QGraphicsItemDatumLabel *>(this->datumLabel);
    
    QPen pen;
    pen.setWidthF((int) 1);
    pen.setStyle(Qt::SolidLine);
    
    QString str = lbl->toPlainText();
    //Relcalculate the measurement based on references stored.  
    const std::vector<App::DocumentObject*> &objects = dim->References.getValues();
    const std::vector<std::string> &SubNames     = dim->References.getSubValues();
           
    if(strcmp(dim->Type.getValueAsString(), "Distance") == 0 || 
       strcmp(dim->Type.getValueAsString(), "DistanceX") == 0 || 
       strcmp(dim->Type.getValueAsString(), "DistanceY") == 0) {
      
        Base::Vector3d p1, p2;  
        if(dim->References.getValues().size() == 1 && SubNames[0].substr(0,4) == "Edge") {
            // Assuming currently just edge

            const Drawing::FeatureViewPart *refObj = static_cast<const Drawing::FeatureViewPart*>(objects[0]);
            int idx = std::atoi(SubNames[0].substr(4,4000).c_str());
            
            DrawingGeometry::BaseGeom *geom = refObj->getCompleteEdge(idx);

            if(geom && geom->geomType == DrawingGeometry::GENERIC ) {
                  DrawingGeometry::Generic *gen = static_cast<DrawingGeometry::Generic *>(geom);
                  Base::Vector2D pnt1 = gen->points.at(0);
                  Base::Vector2D pnt2 = gen->points.at(1);
                  p1 = Base::Vector3d (pnt1.fX, pnt1.fY, 0);
                  p2 = Base::Vector3d (pnt2.fX, pnt2.fY, 0);
                  
            } else {
                delete geom;
                throw Base::Exception("Original edge not found or is invalid type");
            }
            
            // Finished with geomtry so do housekeeping
            delete geom;
            geom = 0;
            
        } else if(dim->References.getValues().size() == 2 && 
                  SubNames[0].substr(0,6) == "Vertex" &&
                  SubNames[1].substr(0,6) == "Vertex") { 
            // Point to Point Dimension
            const Drawing::FeatureViewPart *refObj = static_cast<const Drawing::FeatureViewPart*>(objects[0]);
            int idx = std::atoi(SubNames[0].substr(6,4000).c_str());
            int idx2 = std::atoi(SubNames[1].substr(6,4000).c_str());
        
            DrawingGeometry::Vertex *v1 = refObj->getVertex(idx);
            DrawingGeometry::Vertex *v2 = refObj->getVertex(idx2);
            p1 = Base::Vector3d (v1->pnt.fX, v1->pnt.fY, 0);
            p2 = Base::Vector3d (v2->pnt.fX, v2->pnt.fY, 0);
            
            // Do some house keeping
            delete v1; v1 = 0;
            delete v2; v2 = 0;
            
        } else {
            throw Base::Exception("Invalid reference for dimension type");
        }
        
        Base::Vector3d dir, norm;

        if (strcmp(dim->Type.getValueAsString(), "Distance") == 0 ) {
            dir = (p2-p1);
        } else if (strcmp(dim->Type.getValueAsString(), "DistanceX") == 0 ) {
            dir = Base::Vector3d ( (p2[0] - p1[0] >= FLT_EPSILON) ? 1 : -1, 0, 0);
        } else if (strcmp(dim->Type.getValueAsString(), "DistanceY") == 0 ) {
            dir = Base::Vector3d (0, (p2[1] - p1[1] >= FLT_EPSILON) ? 1 : -1, 0);
        }

        dir.Normalize();
        norm = Base::Vector3d (-dir[1],dir[0], 0);

        // Get magnitude of angle between horizontal
        float angle = atan2f(dir[1],dir[0]);
        bool flip=false;
        if (angle > M_PI_2+M_PI/12) {
            angle -= (float)M_PI;
            flip = true;
        } else if (angle <= -M_PI_2+M_PI/12) {
            angle += (float)M_PI;
            flip = true;
        }
        
        // when the datum line is not parallel to p1-p2 the projection of
        // p1-p2 on norm is not zero, p2 is considered as reference and p1
        // is replaced by its projection p1_
        float normproj12 = (p2-p1)[0] * norm[0] + (p2-p1)[1] * norm[1];
        Base::Vector3d p1_ = p1 + norm * normproj12;

        Base::Vector3d midpos = (p1_ + p2) / 2;    
     
        Base::Vector3d labelPos(lbl->X(), lbl->Y(), 0);
        
        QFontMetrics fm(lbl->font());
        int w = fm.width(str);
        int h = fm.height();

        Base::Vector3d vec = labelPos - p2;        
        float length = vec.x * norm.x + vec.y * norm.y;

        float margin = 3.f;
        float scaler = 1.;
        
        float offset1 = (length + normproj12 < 0) ? -margin : margin;
        float offset2 = (length < 0) ? -margin : margin;
        
        Base::Vector3d perp1 = p1_ + norm * (length + offset1 * scaler);
        Base::Vector3d perp2 = p2  + norm * (length + offset2 * scaler);

        // Calculate the coordinates for the parallel datum lines
        Base::Vector3d  par1 = p1_ + norm * length;
                
        Base::Vector3d  par2 = labelPos - dir * (w / 2 + margin);
        Base::Vector3d  par3 = labelPos + dir * (w / 2 + margin);
        Base::Vector3d  par4 = p2  + norm * length;

        bool flipTriang = false;

        Base::Vector3d del1 = (par3-par1);
        Base::Vector3d del2 = (par2-par1);
        float dot1 = del1.x * dir.x + del1.y * dir.y;
        float dot2 = del2.x * dir.x + del2.y * dir.y;
        //Compare to see if datum label is larger than dimension
        if (dot1 > (par4 - par1).Length()) {
            // Increase Margin to improve visability
            float tmpMargin = 5.f * scaler;
            par3 = par4;
            if(dot2 > (par4 - par1).Length()) {
                par3 = par2;
                par2 = par1 - dir * tmpMargin;
                flipTriang = true;
            }
        } else if (dot2 < 0.f) {
            float tmpMargin = 5.f * scaler;
            par2 = par1;
            if(dot1 < 0.f) {
                par2 = par3;
                par3 = par4 + dir * tmpMargin;
                flipTriang = true;
            }
        }
        
        // Perp Lines
        QPainterPath path;
        path.moveTo(p1[0], p1[1]);
        path.lineTo(perp1[0], perp1[1]);

        path.moveTo(p2[0], p2[1]);
        path.lineTo(perp2[0], perp2[1]);
        
        // Parallel Lines        
        path.moveTo(par1[0], par1[1]);
        path.lineTo(par2[0], par2[1]);
        
        path.moveTo(par3[0], par3[1]);
        path.lineTo(par4[0], par4[1]);
        
        QGraphicsPathItem *arrw = dynamic_cast<QGraphicsPathItem *> (this->arrows);
        arrw->setPath(path);
        arrw->setPen(pen);
        
        // Note Bounding Box size is not the same width or height as text (only used for finding center)
        float bbX  = lbl->boundingRect().width();
        float bbY = lbl->boundingRect().height();
        lbl->setTransformOriginPoint(bbX / 2, bbY /2);
        lbl->setRotation(angle * 180 / M_PI);

//         SbVec3f ar1 = par1 + ((flipTriang) ? -1 : 1) * dir * 0.866f * 2 * margin;
//         SbVec3f ar2 = ar1 + norm * margin;
//                 ar1 -= norm * margin;
// 
//         SbVec3f ar3 = par4 - ((flipTriang) ? -1 : 1) * dir * 0.866f * 2 * margin;
//         SbVec3f ar4 = ar3 + norm * margin ;
//                 ar3 -= norm * margin;

//         //Draw a pretty arrowhead (Equilateral) (Eventually could be improved to other shapes?)
//         glBegin(GL_TRIANGLES);
//           glVertex2f(par1[0], par1[1]);
//           glVertex2f(ar1[0], ar1[1]);
//           glVertex2f(ar2[0], ar2[1]);
// 
//           glVertex2f(par4[0], par4[1]);
//           glVertex2f(ar3[0], ar3[1]);
//           glVertex2f(ar4[0], ar4[1]);
//         glEnd();
    } else if(strcmp(dim->Type.getValueAsString(), "Radius") == 0 ||
              strcmp(dim->Type.getValueAsString(), "Diameter") == 0) {
        // Not sure whether to treat radius and diameter as the same
      
        Base::Vector3d p1, p2, dir; 
        QGraphicsItemDatumLabel *lbl = dynamic_cast<QGraphicsItemDatumLabel *>(this->datumLabel);
        Base::Vector3d labelPos(lbl->X(), lbl->Y(), 0);
    
        double radius;
    
        if(dim->References.getValues().size() == 1 && SubNames[0].substr(0,4) == "Edge") {
            // Assuming currently just edge

            const Drawing::FeatureViewPart *refObj = static_cast<const Drawing::FeatureViewPart*>(objects[0]);
            int idx = std::atoi(SubNames[0].substr(4,4000).c_str());
            
            DrawingGeometry::BaseGeom *geom = refObj->getCompleteEdge(idx);

            if(geom && 
               (geom->geomType == DrawingGeometry::CIRCLE || geom->geomType == DrawingGeometry::ARCOFCIRCLE)) {
                  DrawingGeometry::Circle *circ = static_cast<DrawingGeometry::Circle *>(geom);
                  Base::Vector2D pnt1 = circ->center;
                  radius = circ->radius;
                  p1 = Base::Vector3d (pnt1.fX, pnt1.fY, 0);
            } else {
                throw Base::Exception("Original edge not found or is invalid type");
            }
        } else {
            throw Base::Exception("Invalid reference for dimension type");
        }
        
        // Note Bounding Box size is not the same width or height as text (only used for finding center)
        float bbX  = lbl->boundingRect().width();
        float bbY = lbl->boundingRect().height();
        
        // TODO consider modifying behaviour of SoDatumLabel so position is at center
        // Orientate Position to be at the center of the datumLabel
//         labelPos += 0.5 * Base::Vector3d(bbX, bbY, 0.f);
        dir = (labelPos - p1).Normalize();
        
        // Get magnitude of angle between horizontal
        float angle = atan2f(dir[1],dir[0]);
        bool flip=false;
        if (angle > M_PI_2+M_PI/12) {
            angle -= (float)M_PI;
            flip = true;
        } else if (angle <= -M_PI_2+M_PI/12) {
            angle += (float)M_PI;
            flip = true;
        }
        
        float s = sin(angle);
        float c = cos(angle);
        /*
        
        Base::Console().Log("angle (%f, %f), bbx %f, bby %f", s,c,bbX, bbY);*/
        // Note QGraphicsTextItem takes coordinate system from TOP LEFT - transfer to center
        // Create new coordinate system based around x,y
//         labelPos += 0.5 * Base::Vector3d(bbX * c - bbY * s, bbX * s + bbY * c, 0.f);
//         dir = (labelPos - p1).Normalize();
//         
//         angle = atan2f(dir[1],dir[0]);
//         
        p2 = p1 + dir * radius;
 
        QFontMetrics fm(lbl->font());
            
        int w = fm.width(str);
        int h = fm.height();
                          
        float scaler = 1.;
        float margin = 1.f;
        margin *= scaler;

//         // Create the arrowhead
//         SbVec3f ar0  = p2;
//         SbVec3f ar1  = p2 - dir * 0.866f * 2 * margin;
//         SbVec3f ar2  = ar1 + norm * margin;
//         ar1 -= norm * margin;

        // Calculate the points
        Base::Vector3d pnt1 = labelPos - dir * (margin + w / 2);
        Base::Vector3d pnt2 = labelPos + dir * (margin + w / 2);
        
        if ((labelPos-p1).Length() > (p2-p1).Length())
            p2 = pnt2;
        
        QPainterPath path;
        path.moveTo(p1[0], p1[1]);
        path.lineTo(pnt1[0], pnt1[1]);

        path.moveTo(pnt2[0], pnt2[1]);
        path.lineTo(p2[0], p2[1]);  
        
        QGraphicsPathItem *arrw = dynamic_cast<QGraphicsPathItem *> (this->arrows);
        arrw->setPath(path);
        arrw->setPen(pen);
        
        lbl->setTransformOriginPoint(bbX / 2, bbY /2);
        lbl->setRotation(angle * 180 / M_PI);

        // Draw the Lines
//         glBegin(GL_LINES);
//         glVertex2f(p1[0], p1[1]);
//         glVertex2f(pnt1[0], pnt1[1]);
// 
//         glVertex2f(pnt2[0], pnt2[1]);
//         glVertex2f(p2[0], p2[1]);
//         glEnd();
// 
//         glBegin(GL_TRIANGLES);
//           glVertex2f(ar0[0], ar0[1]);
//           glVertex2f(ar1[0], ar1[1]);
//           glVertex2f(ar2[0], ar2[1]);
//         glEnd();
    }
    
}

QVariant QGraphicsItemViewDimension::itemChange(GraphicsItemChange change, const QVariant &value)
{   
    return QGraphicsItemView::itemChange(change, value);
}

void QGraphicsItemViewDimension::setViewPartFeature(Drawing::FeatureViewDimension *obj)
{
    if(obj == 0)
        return;
    
    this->setViewFeature(static_cast<Drawing::FeatureView *>(obj));

      // Set the QGraphicsItemGroup Properties based on the FeatureView
    float x = obj->X.getValue();
    float y = obj->Y.getValue();

    this->datumLabel->setPos(x, y);    
    updateDim();
    this->draw();
    Q_EMIT dirty();
}

#include "moc_QGraphicsItemViewDimension.cpp"