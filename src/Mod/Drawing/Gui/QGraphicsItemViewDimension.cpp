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

#include <Base/Console.h>
#include <Gui/Command.h>

#include <Mod/Part/App/PartFeature.h>

#include "../App/FeatureViewDimension.h"
#include "../App/FeatureViewPart.h"
#include "QGraphicsItemViewDimension.h"

using namespace DrawingGui;


QGraphicsItemDatumLabel::QGraphicsItemDatumLabel(int ref, QGraphicsScene *scene  ) : reference(ref) 
{
    if(scene) {
        scene->addItem(this);
    }
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
        Q_EMIT dragging();
    }
    
    return QGraphicsItem::itemChange(change, value);
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

    this->arrows  = arrws;
    
    this->datumLabel = dLabel;
         // connecting the needed signals
    QObject::connect(
        dLabel  , SIGNAL(dragging()),
        this    , SLOT  (datumLabelDragged()));
    
    this->addToGroup(arrows);
    this->addToGroup(datumLabel);
}

QGraphicsItemViewDimension::~QGraphicsItemViewDimension()
{
  
}

void QGraphicsItemViewDimension::datumLabelDragged()
{
    int x = this->datumLabel->x();
    int y = this->datumLabel->y();
    Base::Console().Log("Dragging Datum Label %i, %i", this->datumLabel->x(), this->datumLabel->y());
    draw();
}

void QGraphicsItemViewDimension::draw()
{    
    
    // Iterate
    if(this->viewObject == 0 || !this->viewObject->isDerivedFrom(Drawing::FeatureViewDimension::getClassTypeId()))
        return;
        
    Drawing::FeatureViewDimension *dim = dynamic_cast<Drawing::FeatureViewDimension *>(this->viewObject);
    QGraphicsTextItem *datumLbl = dynamic_cast<QGraphicsTextItem *>(this->datumLabel);
    
    
    QPen pen;
    pen.setWidthF((int) 1);
    pen.setStyle(Qt::SolidLine);

    int precision = dim->Precision.getValue();
    QString str;
    str.setNum(dim->getValue(), 'g', precision);
    
    datumLbl->setPlainText(str);

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
            
            DrawingGeometry::BaseGeom *geom = refObj->getEdgeGeometry().at(idx);

            if(geom->geomType == DrawingGeometry::GENERIC ) {
                  DrawingGeometry::Generic *gen = static_cast<DrawingGeometry::Generic *>(geom);
                  Base::Vector2D pnt1 = gen->points.at(0);
                  Base::Vector2D pnt2 = gen->points.at(1);
                  p1 = Base::Vector3d (pnt1.fX, pnt1.fY, 0);
                  p2 = Base::Vector3d (pnt2.fX, pnt2.fY, 0);
            }
        }
        
//         float length = this->param1.getValue();
//         float length2 = this->param2.getValue();
//         const SbVec3f *pnts = this->pnts.getValues(0);

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

        // when the datum line is not parallel to p1-p2 the projection of
        // p1-p2 on norm is not zero, p2 is considered as reference and p1
        // is replaced by its projection p1_
        float normproj12 = (p2-p1)[0] * norm[0] + (p2-p1)[1] * norm[1];
        Base::Vector3d p1_ = p1 + norm * normproj12;

        Base::Vector3d midpos = (p1_ + p2) / 2;

        Base::Vector3d labelPos(this->datumLabel->x(), this->datumLabel->y(), 0);
        
        QFontMetrics fm(datumLbl->font());
        float labelWidth  = datumLbl->boundingRect().width();
        float labelHeight = datumLbl->boundingRect().height();
        
        Base::Vector3d vec = labelPos - p2;
        
        float length = vec.x * norm.x + vec.y * norm.y;

        float offset1 = (length + normproj12 < 0) ? -0.02  : 0.02;
        float offset2 = (length < 0) ? -0.02  : 0.02;

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

//         textOffset = midpos + norm * length + dir * length2;

        float margin = 0.01f;
        float scaler = 1.;
        
        Base::Vector3d perp1 = p1_ + norm * (length + offset1 * scaler);
        Base::Vector3d perp2 = p2  + norm * (length + offset2 * scaler);

        // Calculate the coordinates for the parallel datum lines
        Base::Vector3d  par1 = p1_ + norm * length;
                
        Base::Vector3d  par2 = labelPos - norm * labelHeight / 2 - dir * (labelWidth -margin);
        Base::Vector3d  par3 = labelPos - norm * labelHeight / 2 - dir * (margin);
        Base::Vector3d  par4 = p2  + norm * length;

//         bool flipTriang = false;
// 
//         if ((par3-par1).dot(dir) > (par4 - par1).length()) {
//             // Increase Margin to improve visability
//             float tmpMargin = 0.08f * scaler;
//             par3 = par4;
//             if((par2-par1).dot(dir) > (par4 - par1).length()) {
//                 par3 = par2;
//                 par2 = par1 - dir * tmpMargin;
//                 flipTriang = true;
//             }
//         } else if ((par2-par1).dot(dir) < 0.f) {
//             float tmpMargin = 0.08f * scaler;
//             par2 = par1;
//             if((par3-par1).dot(dir) < 0.f) {
//                 par2 = par3;
//                 par3 = par4 + dir * tmpMargin;
//                 flipTriang = true;
//             }
//         }
        
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
        
        datumLbl->setRotation(angle * 180 / M_PI);

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
    this->draw();
    Q_EMIT dirty();
}

#include "moc_QGraphicsItemViewDimension.cpp"