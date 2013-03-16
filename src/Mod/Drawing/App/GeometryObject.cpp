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

# include "PreCompiled.h"
#ifndef _PreComp_
# include <sstream>
# include <BRepAdaptor_Curve.hxx>
# include <Geom_Circle.hxx>
# include <gp_Circ.hxx>
# include <gp_Elips.hxx>

# include <Bnd_Box.hxx>
# include <BRepBndLib.hxx>
# include <BRepBuilderAPI_Transform.hxx>
# include <HLRBRep_Algo.hxx>

# include <TopoDS_Shape.hxx>
# include <HLRTopoBRep_OutLiner.hxx>
//# include <BRepAPI_MakeOutLine.hxx>
# include <HLRAlgo_Projector.hxx>
# include <HLRBRep_ShapeBounds.hxx>
# include <HLRBRep_EdgeData.hxx>
# include <HLRBRep_HLRToShape.hxx>
# include <HLRBRep_Data.hxx>
# include <gp_Ax2.hxx>
# include <gp_Pnt.hxx>
# include <gp_Dir.hxx>
# include <gp_Vec.hxx>
# include <Poly_Polygon3D.hxx>
# include <Poly_Triangulation.hxx>
# include <Poly_PolygonOnTriangulation.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Face.hxx>
# include <TopoDS_Edge.hxx>
# include <TopoDS_Vertex.hxx>
# include <TopExp.hxx>
# include <TopExp_Explorer.hxx>
# include <TopTools_ListIteratorOfListOfShape.hxx>
# include <TopTools_IndexedMapOfShape.hxx>
# include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
# include <TopTools_ListOfShape.hxx>
# include <TColgp_Array1OfPnt2d.hxx>

# include <BRep_Tool.hxx>
# include <BRepMesh.hxx>
# include <BRep_Builder.hxx>
# include <BRepBuilderAPI_MakeWire.hxx>
# include <BRepTools_WireExplorer.hxx>
# include <ShapeFix_Wire.hxx>

# include <BRepAdaptor_CompCurve.hxx>
# include <HLRBRep.hxx>
# include <HLRAlgo_EdgeIterator.hxx>
# include <Handle_BRepAdaptor_HCompCurve.hxx>
# include <Approx_Curve3d.hxx>
# include <BRepAdaptor_HCurve.hxx>
# include <Handle_BRepAdaptor_HCurve.hxx>
# include <Handle_HLRBRep_Data.hxx>
# include <Geom_BSplineCurve.hxx>
# include <Handle_Geom_BSplineCurve.hxx>
# include <Geom_BezierCurve.hxx>
# include <GeomConvert_BSplineCurveToBezierCurve.hxx>
# include <GeomConvert_BSplineCurveKnotSplitting.hxx>
# include <Geom2d_BSplineCurve.hxx>
#include <boost/graph/graph_concepts.hpp>
#endif

# include <Base/Console.h>
# include <Base/Exception.h>
# include <Base/FileInfo.h>
# include <Base/Tools.h>

# include <Mod/Part/App/PartFeature.h>

# include "GeometryObject.h"
# include "ProjectionAlgos.h"

using namespace DrawingGeometry;

struct EdgePoints {
    gp_Pnt v1, v2;
    TopoDS_Edge edge;
};

GeometryObject::GeometryObject() : brep_hlr(0), Tolerance(0.05f)
{    
}

GeometryObject::~GeometryObject()
{
    this->clear();
}

void GeometryObject::setTolerance(double value)
{
    this->Tolerance = value;
}

void GeometryObject::clear()
{
           
    for(std::vector<BaseGeom *>::iterator it = edgeGeom.begin(); it != edgeGeom.end(); ++it) {
        delete *it;
        *it = 0;
    }
        
    for(std::vector<Face *>::iterator it = faceGeom.begin(); it != faceGeom.end(); ++it) {
        delete *it;
        *it = 0;
    }
    
    for(std::vector<Vertex *>::iterator it = vertexGeom.begin(); it != vertexGeom.end(); ++it) {
        delete *it;
        *it = 0;
    }
    
    vertexGeom.clear();
    vertexReferences.clear();
    
    faceGeom.clear();    
    faceReferences.clear();
    
    edgeGeom.clear();
    edgeReferences.clear();
    
    if(brep_hlr)
        brep_hlr->Delete();
}

TopoDS_Shape GeometryObject::invertY(const TopoDS_Shape& shape)
{
    // make sure to have the y coordinates inverted
    gp_Trsf mat;
    Bnd_Box bounds;
    BRepBndLib::Add(shape, bounds);
    bounds.SetGap(0.0);
    Standard_Real xMin, yMin, zMin, xMax, yMax, zMax;
    bounds.Get(xMin, yMin, zMin, xMax, yMax, zMax);
    mat.SetMirror(gp_Ax2(gp_Pnt((xMin+xMax)/2,(yMin+yMax)/2,(zMin+zMax)/2), gp_Dir(0,1,0)));
    BRepBuilderAPI_Transform mkTrf(shape, mat);
    return mkTrf.Shape();
}


void GeometryObject::drawFace (const bool visible, const int typ, const int iface, Handle_HLRBRep_Data & DS, TopoDS_Shape& Result) const
{
    HLRBRep_FaceIterator Itf;

    for (Itf.InitEdge(DS->FDataArray().ChangeValue(iface)); Itf.MoreEdge(); Itf.NextEdge()) {               
      
      int ie = Itf.Edge();
      
      HLRBRep_EdgeData& edf = DS->EDataArray().ChangeValue(ie);
      
      if (true/*!edf.Used()*/) {        
        bool todraw;
        if (typ == 1) 
            todraw =  Itf.IsoLine();
        else if (typ == 2)
            todraw =  Itf.Internal();
        else if (typ == 3) 
            todraw =  edf.Rg1Line() &&
          !edf.RgNLine() && !Itf.OutLine();
        else if (typ == 4) 
            todraw =  edf.RgNLine() && !Itf.OutLine();
        else              
            todraw = !(Itf.IsoLine()  || Itf.Internal() || (edf.Rg1Line() && !Itf.OutLine()));

      if (true) {
          // Draw all the edges in the face
        double sta,end;
        float tolsta,tolend;
        
        BRep_Builder B;
        TopoDS_Edge E;
        HLRAlgo_EdgeIterator It;
        
            for (It.InitVisible(edf.Status()); It.MoreVisible(); It.NextVisible()) {
                It.Visible(sta,tolsta,end,tolend);
                
                E = HLRBRep::MakeEdge(edf.Geometry(),sta,end);
                if (!E.IsNull()) {
                    B.Add(Result,E);
                }
                edf.Used(false);
            }

            for (It.InitHidden(edf.Status()); It.MoreHidden(); It.NextHidden()) {

                It.Hidden(sta,tolsta,end,tolend);
                E = HLRBRep::MakeEdge(edf.Geometry(),sta,end);
                if (!E.IsNull()) {
                    B.Add(Result,E);
                }          
                edf.Used(false);
            }
          
        } else {
          if(typ > 4 && (edf.Rg1Line() && !Itf.OutLine())) {
            int hc = edf.HideCount();
            if(hc > 0) {
              edf.Used(true);
            } else {
              ++hc;
              edf.HideCount(hc); //to try with another face
            }
          } else {
            edf.Used(true);
          }
        }
      }
    }
}

void GeometryObject::drawEdge(HLRBRep_EdgeData& ed, TopoDS_Shape& Result, const bool visible) const
{
    double sta,end;
    float tolsta,tolend;
    
    BRep_Builder B;
    TopoDS_Edge E;
    HLRAlgo_EdgeIterator It;
    
    if (visible) {
        for(It.InitVisible(ed.Status()); It.MoreVisible(); It.NextVisible()) {
            It.Visible(sta,tolsta,end,tolend);
            
            E = HLRBRep::MakeEdge(ed.Geometry(),sta,end);
            if (!E.IsNull()) {
                B.Add(Result,E);
            }
        }
    } else {
        for(It.InitHidden(ed.Status()); It.MoreHidden(); It.NextHidden()) {
            It.Hidden(sta,tolsta,end,tolend);
            E = HLRBRep::MakeEdge(ed.Geometry(),sta,end);
            if (!E.IsNull()) {
                B.Add(Result,E);
            }          
        }
    }
}

void GeometryObject::extractFaces(HLRBRep_Algo *myAlgo, const TopoDS_Shape &S, int type, bool visible, ExtractionType extractionType)
{
    if(!myAlgo)
        return;

    Handle_HLRBRep_Data DS = myAlgo->DataStructure();
    if (DS.IsNull())
        return;
     
    DS->Projector().Scaled(true);
    
    int e1 = 1;
    int e2 = DS->NbEdges();
    int f1 = 1;
    int f2 = DS->NbFaces();
    
    if (!S.IsNull()) {
        Standard_Integer v1,v2;
        Standard_Integer index = myAlgo->Index(S);
        if(index == 0)
            return;
        myAlgo->ShapeBounds(index).Bounds(v1,v2,e1,e2,f1,f2);    
    }
    
    TopTools_IndexedMapOfShape anfIndices;    
    
    TopTools_IndexedMapOfShape& Faces = DS->FaceMap();
     
    TopExp::MapShapes(S, TopAbs_FACE, anfIndices);

    Base::Console().Log("Num face: %i", anfIndices.Extent());
    Base::Console().Log("Num faces gen: %i", Faces.Extent());
    
    BRep_Builder B;

    /* ----------------- Extract Faces ------------------ */    
    for (int iface = f1; iface <= f2; iface++) {       
        TopoDS_Shape face;
        B.MakeCompound(TopoDS::Compound(face));

        drawFace(visible,5,iface,DS,face);
        

        std::list<TopoDS_Wire> wireList;
        
        createWire(face, wireList);
        
        // Process the wires
        if (wireList.size() > 0) { 
            // FIXME: The right way here would be to determine the outer and inner wires and
            // generate a face with holes (inner wires have to be taged REVERSE or INNER).
            // thats the only way to transport a somwhat more complex sketch...
            //result = *wires.begin();

            DrawingGeometry::Face *myFace = new DrawingGeometry::Face();

            // Iterate wire list and build up each wires and store the in Geometry::Wire
            for (std::list<TopoDS_Wire>::iterator wt = wireList.begin(); wt != wireList.end(); ++wt) {
                // Explore all edges of input and calculate base geometry representation
              
              

                              // Create the geometry and store
              DrawingGeometry::Wire *wire = new DrawingGeometry::Wire;
              BRepTools_WireExplorer edges(*wt);
              
              for (int i = 1 ; edges.More(); edges.Next(),i++) {
                  BRep_Builder builder;
                  TopoDS_Compound comp;
                  builder.MakeCompound(comp);
                  builder.Add(comp, edges.Current());     
                  int edgesAdded = calculateGeometry(comp, extractionType, wire->geoms);    
              }
                     

              
              
//               if(edgesAdded > 0)
                 myFace->wires.push_back(wire);
            }

            // Proces each wire
            faceGeom.push_back(myFace);  
            
            int idxFace;
            for (int i = 1; i <= anfIndices.Extent(); i++) {
                idxFace = Faces.FindIndex(anfIndices(iface));
                if (idxFace != 0) {
                    break;
                }
            }
            
            if(idxFace == 0)
                idxFace = -1; // If Face not found - select hidden
            
            // Push the found face index onto references stack
            faceReferences.push_back(idxFace);
            }  
    }

    DS->Projector().Scaled(false);
}

bool GeometryObject::shouldDraw(const bool inFace, const int typ, HLRBRep_EdgeData& ed)
{
    bool todraw = false;
    if(inFace)
        todraw = true;
    else if (typ == 3) 
        todraw = ed.Rg1Line() && !ed.RgNLine();
    else if (typ == 4)
        todraw = ed.RgNLine();
    else              
        todraw =!ed.Rg1Line();

    return todraw;
}

void GeometryObject::extractVerts(HLRBRep_Algo *myAlgo, const TopoDS_Shape &S, HLRBRep_EdgeData& ed, int ie, ExtractionType extractionType)
{
    if(!myAlgo)
        return;

    Handle_HLRBRep_Data DS = myAlgo->DataStructure();

    if (DS.IsNull())
      return;

    DS->Projector().Scaled(true);

 
    TopTools_IndexedMapOfShape anIndices;
    TopTools_IndexedMapOfShape anvIndices;

    TopExp::MapShapes(S, TopAbs_EDGE, anIndices);
    TopExp::MapShapes(S, TopAbs_VERTEX, anvIndices);

    Base::Console().Log("Num Vertices: %i",  anvIndices.Extent());
    Base::Console().Log("Num edges: %i",  anIndices.Extent());
    
    // Load the edge
    if(ie < 0) {
      
    } else {
        TopoDS_Shape shape = anIndices.FindKey(ie);
        TopoDS_Edge edge = TopoDS::Edge(shape);
        
        // Gather a list of points associated with this curve
        std::list<TopoDS_Shape> edgePoints;
        
        TopExp_Explorer xp;       
        xp.Init(edge,TopAbs_VERTEX);
        while(xp.More()) {
            edgePoints.push_back(xp.Current());
            xp.Next();
        }
        for(std::list<TopoDS_Shape>::const_iterator it = edgePoints.begin(); it != edgePoints.end(); ++it) {
          
            // Should share topological data structure so can reference
            int iv = anvIndices.FindIndex(*it); // Index of the found vertex
            
            if(iv < 0)
                continue;
            
            // Check if vertex has alrady been addded
            std::vector<int>::iterator vert;
            vert = std::find(vertexReferences.begin(), vertexReferences.end(), iv);
            
            if(vert == vertexReferences.end()) {
              
                // If the index was found and is unique, the point is projected using the HLR Projector Algorithm
                gp_Pnt2d prjPnt;
                DS->Projector().Project(BRep_Tool::Pnt(TopoDS::Vertex(*it)), prjPnt);
                
                // Check if this point lies on a visible section of the projected curve  
                double sta,end;
                float tolsta,tolend;
                
                // There will be multiple edges that form the total edge so collect these
                BRep_Builder B;
                TopoDS_Compound comp;
                B.MakeCompound(comp);

                TopoDS_Edge E;
                HLRAlgo_EdgeIterator It;
                
                for(It.InitVisible(ed.Status()); It.MoreVisible(); It.NextVisible()) {
                    It.Visible(sta,tolsta,end,tolend);
                    
                    E = HLRBRep::MakeEdge(ed.Geometry(),sta,end);
                    if (!E.IsNull()) {
                        B.Add(comp,E);
                    }
                }
                
                bool vertexVisible = false;
                TopExp_Explorer exp;       
                exp.Init(comp,TopAbs_VERTEX);
                while(exp.More()) {
                    
                    gp_Pnt pnt = BRep_Tool::Pnt(TopoDS::Vertex(exp.Current()));
                    gp_Pnt2d edgePnt(pnt.X(), pnt.Y());
                    if(edgePnt.SquareDistance(prjPnt) < Precision::Confusion()) {
                        vertexVisible = true;
                        break;
                    }
                    exp.Next();
                }
    
                if(vertexVisible) {
                    Vertex *myVert = new Vertex(prjPnt.X(), prjPnt.Y());
                    vertexGeom.push_back(myVert);
                    vertexReferences.push_back(iv);
                }
            }
        }        
    }         
}

void GeometryObject::extractEdges(HLRBRep_Algo *myAlgo, const TopoDS_Shape &S, int type, bool visible, ExtractionType extractionType)
{
    if (!myAlgo)
      return;

    Handle_HLRBRep_Data DS = myAlgo->DataStructure();

    if (DS.IsNull())
        return;

    DS->Projector().Scaled(true);
    
    int e1 = 1;
    int e2 = DS->NbEdges();
    int f1 = 1;
    int f2 = DS->NbFaces();
    
    if (!S.IsNull()) {
        int v1,v2;
        int index = myAlgo->Index(S);
        if(index == 0)
            return;
        myAlgo->ShapeBounds(index).Bounds(v1,v2,e1,e2,f1,f2);    
    }

    HLRBRep_EdgeData* ed = &(DS->EDataArray().ChangeValue(e1 - 1));

    // Get map of edges and faces from projected geometry
    TopTools_IndexedMapOfShape& Edges = DS->EdgeMap();    
    TopTools_IndexedMapOfShape anIndices;
    
    TopExp::MapShapes(S, TopAbs_EDGE, anIndices);

    Base::Console().Log("Num edges: %i",  anIndices.Extent());
    Base::Console().Log("Num edges gen: %i",  Edges.Extent());   
    
    for (int j = e1; j <= e2; j++) {
        ed++;
        if (ed->Selected() && !ed->Vertical()) {
            ed->Used(false);
            ed->HideCount(0);
            
        } else {
            ed->Used(true); 
        }
    }
    
    BRep_Builder B;
    
    /* ----------------- Extract Edges ------------------ */    
    for (int i = 1; i <= anIndices.Extent(); i++) {
      int ie = Edges.FindIndex(anIndices(i));
      
      if (ie != 0) {
        
          HLRBRep_EdgeData& ed = DS->EDataArray().ChangeValue(ie);
          if(!ed.Used()) {           
              if(shouldDraw(false, type, ed)) {
                
                  TopoDS_Shape result;
                  B.MakeCompound(TopoDS::Compound(result));
                  
                  drawEdge(ed, result, visible);
                  
                  // Extract and Project Vertices
                  extractVerts(myAlgo, S, ed, ie, extractionType);

                  int edgesAdded = calculateGeometry(result, extractionType, edgeGeom);
                  
                  // Push the edge references 
                  while(edgesAdded--)
                      edgeReferences.push_back(ie);
                  
              }
                
              ed.Used(true); 
          }          
      }
    }
    
    // Add any remaining edges that couldn't be found
    HLRBRep_EdgeData* edge = &(DS->EDataArray().ChangeValue(e1 - 1));
    int edgeIdx = -1; // Negative index for edge referencesz
    for (int ie = e1; ie <= e2; ie++) {     

      HLRBRep_EdgeData& ed = DS->EDataArray().ChangeValue(ie);
      if (!ed.Used()) {          
          if(shouldDraw(false, type, ed)) {
              TopoDS_Shape result;
              B.MakeCompound(TopoDS::Compound(result));
              
              drawEdge(ed, result, visible);
              int edgesAdded = calculateGeometry(result, extractionType, edgeGeom);
              
              // Push the edge references 
              while(edgesAdded--)
                  edgeReferences.push_back(edgeIdx);
          }
          ed.Used(true);
      }
    }

    DS->Projector().Scaled(false);
}

void GeometryObject::createWire(const TopoDS_Shape &input, std::list<TopoDS_Wire> &wires) const
{    
     if(input.IsNull())
        return; // There is no OpenCascade Geometry to be calculated

    std::list<TopoDS_Edge> edgeList;
    
    // Explore all edges of input and calculate base geometry representation
    TopExp_Explorer edges(input, TopAbs_EDGE);
    for (int i = 1 ; edges.More(); edges.Next(),i++) {
        const TopoDS_Edge& edge = TopoDS::Edge(edges.Current());
        edgeList.push_back(edge);
    }

    // sort them together to wires
    while (edgeList.size() > 0) {
        BRepBuilderAPI_MakeWire mkWire;
        // add and erase first edge
        TopoDS_Edge e = TopoDS_Edge(edgeList.front());
        TopExp_Explorer xp;
        EdgePoints ep;
        xp.Init(edgeList.front(),TopAbs_VERTEX);
        ep.v1 = BRep_Tool::Pnt(TopoDS::Vertex(xp.Current()));
        xp.Next();
        ep.v2 = BRep_Tool::Pnt(TopoDS::Vertex(xp.Current()));
        ep.edge = edgeList.front();
        
                    Base::Console().Log("<%f %f %f>, <%f %f %f> \n", ep.v1.X(),ep.v1.Y(),ep.v1.Z(), ep.v2.X(),ep.v2.Y(),ep.v2.Z());
                    
        mkWire.Add(edgeList.front());
       

        std::list<TopoDS_Edge> sortedWire;
         edgeList.pop_front();
         
        TopoDS_Wire new_wire = mkWire.Wire(); // current new wire
      
        
        // try to connect each edge to the wire, the wire is complete if no more egdes are connectible
        bool found = false;
        do {
            found = false;
            for (std::list<TopoDS_Edge>::iterator pE = edgeList.begin(); pE != edgeList.end(); ++pE) {
                mkWire.Add(*pE);
                if (mkWire.Error() != BRepBuilderAPI_DisconnectedWire) {
                    // edge added ==> remove it from list
                    found = true;
                    TopExp_Explorer xp;
   
        EdgePoints ep;
        xp.Init(*pE,TopAbs_VERTEX);
        ep.v1 = BRep_Tool::Pnt(TopoDS::Vertex(xp.Current()));
        xp.Next();
        ep.v2 = BRep_Tool::Pnt(TopoDS::Vertex(xp.Current()));
        ep.edge = *pE;
        
                    Base::Console().Log("<%f %f %f>, <%f %f %f> \n", ep.v1.X(),ep.v1.Y(),ep.v1.Z(), ep.v2.X(),ep.v2.Y(),ep.v2.Z());
                    edgeList.erase(pE);
                    new_wire = mkWire.Wire();
                    break;
                }
            }
        }
        while (found);
       
        wires.push_back(new_wire);
    }
}

void GeometryObject::extractGeometry(const TopoDS_Shape &input, const Base::Vector3f &direction)
{
    // Clear previous Geometry and References that may have been stored
    this->clear();

    const TopoDS_Shape invertShape = invertY(input);
    HLRBRep_Algo *brep_hlr = new HLRBRep_Algo();
    brep_hlr->Add(invertShape);

    try {
        #if defined(__GNUC__) && defined (FC_OS_LINUX)
        Base::SignalException se;
        #endif
        gp_Ax2 transform(gp_Pnt(0,0,0),gp_Dir(direction.x,direction.y,direction.z));
        HLRAlgo_Projector projector( transform );

        brep_hlr->Projector(projector);
        brep_hlr->Update();
        brep_hlr->Hide();

    }
    catch (...) {
        Standard_Failure::Raise("Fatal error occurred while projecting shape");
    }

    // extracting the result sets:

    // V  = shapes.VCompound       ();// hard edge visibly
    // V1 = shapes.Rg1LineVCompound();// Smoth edges visibly
    // VN = shapes.RgNLineVCompound();// contour edges visibly
    // VO = shapes.OutLineVCompound();// contours apparents visibly
    // VI = shapes.IsoLineVCompound();// isoparamtriques   visibly
    // H  = shapes.HCompound       ();// hard edge       invisibly
    // H1 = shapes.Rg1LineHCompound();// Smoth edges  invisibly
    // HN = shapes.RgNLineHCompound();// contour edges invisibly
    // HO = shapes.OutLineHCompound();// contours apparents invisibly
    // HI = shapes.IsoLineHCompound();// isoparamtriques   invisibly

    // Extract Hidden Edges
    //extractCompound(brep_hlr, invertShape, 5, false, WithHidden);// Hard Edge
//     calculateGeometry(extractCompound(brep_hlr, invertShape, 2, false), WithHidden); // Outline
//     calculateGeometry(extractCompound(brep_hlr, invertShape, 3, false), (ExtractionType)(WithSmooth | WithHidden)); // Smooth

    // Extract Visible Edges
     extractEdges(brep_hlr, invertShape, 5, true, WithSmooth);  // Hard Edge   
    
      // Extract Faces
//       !extractFaces(brep_hlr, invertShape, 5, true, WithSmooth);  // 
//     calculateGeometry(extractCompound(brep_hlr, invertShape, 2, true), Plain);  // Outline
//     calculateGeometry(extractCompound(brep_hlr, invertShape, 3, true), WithSmooth); // Smooth Edge
}

int GeometryObject::calculateGeometry(const TopoDS_Shape &input, const ExtractionType extractionType, std::vector<BaseGeom *> &geom)
{
    if(input.IsNull())
        return 0; // There is no OpenCascade Geometry to be calculated

    // build a mesh to explore the shape
    BRepMesh::Mesh(input, this->Tolerance);

    int geomsAdded = 0;

    // Explore all edges of input and calculate base geometry representation
    TopExp_Explorer edges(input, TopAbs_EDGE);
    for (int i = 1 ; edges.More(); edges.Next(),i++) {
        const TopoDS_Edge& edge = TopoDS::Edge(edges.Current());
        BRepAdaptor_Curve adapt(edge);

        switch(adapt.GetType()) {
          case GeomAbs_Circle: {
            double f = adapt.FirstParameter();
            double l = adapt.LastParameter();
            gp_Pnt s = adapt.Value(f);
            gp_Pnt e = adapt.Value(l);

            if (fabs(l-f) > 1.0 && s.SquareDistance(e) < 0.001) {
                  Circle *circle = new Circle(adapt);
                  circle->extractType = extractionType;
                  geom.push_back(circle);
            } else {
                  AOC *aoc = new AOC(adapt);
                  aoc->extractType = extractionType;
                  geom.push_back(aoc);
            }
          } break;
          case GeomAbs_Ellipse: {
            double f = adapt.FirstParameter();
            double l = adapt.LastParameter();
            gp_Pnt s = adapt.Value(f);
            gp_Pnt e = adapt.Value(l);
            if (fabs(l-f) > 1.0 && s.SquareDistance(e) < 0.001) {
                  Ellipse *ellipse = new Ellipse(adapt);
                  ellipse->extractType = extractionType;
                  geom.push_back(ellipse);
            } else {
                  AOE *aoe = new AOE(adapt);
                  aoe->extractType = extractionType;
                  geom.push_back(aoe);
            }
          } break;
          case GeomAbs_BSplineCurve: {
            BSpline *bspline = new BSpline(adapt);
            bspline->extractType = extractionType;
            geom.push_back(bspline);
          } break;
          default: {
            Generic *primitive = new Generic(adapt);
            primitive->extractType = extractionType;
            geom.push_back(primitive);
          }  break;
        }
        geomsAdded++;
    }
    return geomsAdded;
}

