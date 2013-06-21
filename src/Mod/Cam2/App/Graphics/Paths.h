
#pragma once

#include <TopoDS_Edge.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Ax2.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <gp_Pln.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <TopoDS.hxx>
#include <Bnd_Box.hxx>

#include "Area.h"

namespace Cam
{
	TopoDS_Edge Edge( const gp_Pnt start, const gp_Pnt end, const gp_Circ circle );
	TopoDS_Edge Edge( const gp_Pnt start, const gp_Pnt end );
	TopoDS_Edge Edge( const TopoDS_Edge original_edge, const Standard_Real start_u, const Standard_Real end_u );

	double AngleBetweenVectors(
			const gp_Pnt & vector_1_start_point,
			const gp_Pnt & vector_1_end_point,
			const gp_Pnt & vector_2_start_point,
			const gp_Pnt & vector_2_end_point,
			const double minimum_angle );

	double AngleBetweenVectors(
			const gp_Vec & vector_1,
			const gp_Vec & vector_2,
			const gp_Vec & reference,
			const double minimum_angle );

	double GetTolerance();

	typedef std::list<TopoDS_Wire> Wires_t;
	typedef std::list<TopoDS_Face> Faces_t;

	/**
		Define a class that encapsulates a coordinate and its own coordinate system.

		This allows the application to add/subtract offsets from each axis and the
		actual point's location will change based on that adjustment within the
		context of the coordinate system (i.e. rotation)  It's really just about
		making the application layer a little easier to read.

		eg: gp_Ax2 ax2(......)
			Point point(ax2);
			point.XAdjustment( 3.4 );	// Add 3.4 to the current X coordinate.  This change
										// may well result in an adjustment to any/all of the
										// X,Y or Z values based on the rotation of ax2.
			gp_Pnt final_location( point.Location() );
	 */
	class Point : public gp_Ax2
	{
	public:
		Point(const gp_Ax2 ax2) : gp_Ax2(ax2) { }
		Point(const gp_Pnt p);
		Point(const double x, const double y, const double z);
		Point(const gp_Pln plane, const double x, const double y, const double z);
		Point(const gp_Pln plane);
		Point(const Point & rhs) : gp_Ax2(rhs) { }
		Point & operator= ( const Point & rhs ) { if (this != &rhs) { gp_Ax2::operator=( rhs ); } return(*this); }
		gp_Pnt Location() const { return(gp_Ax2::Location()); }
		Point & Location( const gp_Pnt p );

	public:
		Point & Adjustment( const double x_offset, const double y_offset, const double z_offset );
		Point & operator+= ( const gp_XYZ offset );
		Point & Adjustment( const gp_Dir direction, const double distance );

		Point & XAdjustment( const double offset );
		Point & YAdjustment( const double offset );
		Point & ZAdjustment( const double offset );

		Point & SetX( const double value );
		Point & SetY( const double value );
		Point & SetZ( const double value );

	public:
		double X(const bool in_drawing_units = false) const
		{
			if (in_drawing_units == false) return(gp_Ax2::Location().X());
			else return(gp_Ax2::Location().X() / Units());
		}
		double Y(const bool in_drawing_units = false) const
		{
			if (in_drawing_units == false) return(gp_Ax2::Location().Y());
			else return(gp_Ax2::Location().Y() / Units());
		}
		double Z(const bool in_drawing_units = false) const
		{
			if (in_drawing_units == false) return(gp_Ax2::Location().Z());
			else return(gp_Ax2::Location().Z() / Units());
		}

		double Units() const
		{
			return(1.0);	// TODO Handle metric or imperial units (25.4 for inches)
		}

		double Tolerance() const
		{
			if (s_tolerance <= 0.0)
			{
				s_tolerance = Cam::GetTolerance();
			}
			return(s_tolerance);
		}

		bool operator==( const Point & rhs ) const
		{
			// We use the sum of both point's tolerance values.
			return(gp_Ax2::Location().Distance(rhs.Location()) <= (Tolerance() + rhs.Tolerance()));
		} // End equivalence operator

		bool operator!=( const Point & rhs ) const
		{
			return(! (*this == rhs));
		} // End not-equal operator

		bool operator<( const Point & rhs ) const
		{
			if (*this == rhs) return(false);

			if (fabs(X() - rhs.X()) > (Tolerance() + rhs.Tolerance()))
			{
				if (X() > rhs.X()) return(false);
				if (X() < rhs.X()) return(true);
			}

			if (fabs(Y() - rhs.Y()) > (Tolerance() + rhs.Tolerance()))
			{
				if (Y() > rhs.Y()) return(false);
				if (Y() < rhs.Y()) return(true);
			}

			if (fabs(Z() - rhs.Z()) > (Tolerance() + rhs.Tolerance()))
			{
				if (Z() > rhs.Z()) return(false);
				if (Z() < rhs.Z()) return(true);
			}

			return(false);	// They're equal
		} // End equivalence operator

		void ToDoubleArray( double *pArrayOfThree ) const
		{
			pArrayOfThree[0] = X();
			pArrayOfThree[1] = Y();
			pArrayOfThree[2] = Z();
		} // End ToDoubleArray() method


		double XYDistance( const Point & rhs ) const
		{
			gp_Pnt _lhs(this->Location());
			gp_Pnt _rhs(rhs.Location());
			_lhs.SetZ(0.0);
			_rhs.SetZ(0.0);
			return(_lhs.Distance(_rhs));
		}

		double XZDistance( const Point & rhs ) const
		{
			gp_Pnt _lhs(this->Location());
			gp_Pnt _rhs(rhs.Location());
			_lhs.SetY(0.0);
			_rhs.SetY(0.0);
			return(_lhs.Distance(_rhs));
		}

		double YZDistance( const Point & rhs ) const
		{
			gp_Pnt _lhs(this->Location());
			gp_Pnt _rhs(rhs.Location());
			_lhs.SetX(0.0);
			_rhs.SetX(0.0);
			return(_lhs.Distance(_rhs));
		}

	private:
		static double s_tolerance;
	}; // End Point class definition


	std::list<Cam::Point> Corners(Bnd_Box box);
	Standard_Real LargestBoxDimension(Bnd_Box box);


	/**
		Define an Ax3 class simply to provide an equivalence
		operator for the gp_Ax3 class.  If we don't need to rotate
		a Path from one plane to another then we should avoid doing so.  It's
		expensive in terms of CPU time to do so.  If the path's plane
		is the same as the XY plane anyway then there's no need to adjust it.
	 */
	class Ax3 : public gp_Ax3
	{
	public:
		Ax3( const gp_Ax3 & rhs ) : gp_Ax3(rhs) {}
		bool operator== ( const Ax3 & rhs ) const;
		bool operator!= ( const Ax3 & rhs ) const;
	};

	bool IsValid(const TopoDS_Shape shape);	// is non-null and has length > tolerance.
	Standard_Real Area(const TopoDS_Shape shape);
	std::list<TopoDS_Face> IntersectFaces( const TopoDS_Face lhs, const TopoDS_Face rhs );
	std::list<TopoDS_Face> SubtractFaces( const TopoDS_Face lhs, const TopoDS_Face rhs );
	Faces_t UnionFaces( const TopoDS_Face lhs, const TopoDS_Face rhs );
	Faces_t UnionFaces( Faces_t faces );
	bool FacesIntersect( const TopoDS_Face lhs, const TopoDS_Face rhs );

	class Paths;	// Forward declaration.

	/**
		Container for a single TopoDS_Edge along with all its various parameters.
		NOTE: The OpenCascade library uses the terms FirstParameter() and LastParameter()
		to define a value that is representative of the first end of the edge and the last
		end of the edge.  This class uses the terms StartParameter() and StartPoint() etc. to
		indicate the starting point in terms of movement along the edge.  This distinction
		is the based on the m_is_forwards boolean.  We may want to 'reverse' this edge
		in terms of tool path.  This is simply a reversal of the m_is_forwards boolean
		which allows us to select either the FirstParameter or the LastParameter as the
		StartParameter as appropriate.  I know the terminology is a little confusing but
		part of it comes from OpenCascade and the other part just makes sense in terms
		of tool path progression.
	 */
	class Path
	{
	public:
		Path(const TopoDS_Edge edge): m_edge(edge), m_is_forwards(true), m_length(-1), m_tolerance(-1)
		{
			
		}

		Path & operator= ( const Path & rhs )
		{
			if (this != &rhs)
			{
				BRepBuilderAPI_Copy duplicate;
				duplicate.Perform(rhs.m_edge);
				m_edge = TopoDS::Edge(duplicate.Shape());
				m_length = rhs.m_length;
				m_is_forwards = rhs.m_is_forwards;
				m_tolerance = rhs.m_tolerance;
				m_pCurve.reset(NULL);
			}

			return(*this);
		}

		Path( const Path & rhs )
		{
			*this = rhs;	// call the assignment operator.
		}

		TopoDS_Edge Edge() const 
		{ 
			BRepBuilderAPI_Copy duplicate;
			duplicate.Perform(m_edge);
			return(TopoDS::Edge(duplicate.Shape())); 
		}

		Standard_Real StartParameter() const;
		Standard_Real EndParameter() const;

		Standard_Real Proportion( const double proportion ) const;

		Cam::Point PointAt(const Standard_Real U) const
		{
			gp_Pnt point;
			Curve().D0(U,point);
			return(point);
		}

		Cam::Point PointAtDistance(const Standard_Real distance) const
		{
			Standard_Real U = Proportion(distance/Length());
			return(PointAt(U));
		}

		void D0( const Standard_Real U, gp_Pnt &P) const
		{
			Curve().D0(U,P);
		}

		void D1( const Standard_Real U, gp_Pnt &P, gp_Vec &V ) const
		{
			Curve().D1(U,P,V);
		}

		Cam::Point StartPoint() const
		{
			gp_Pnt point;
			Curve().D0(StartParameter(),point);
			return(point);
		}

		Cam::Point MidPoint() const
		{
			gp_Pnt point;
			Curve().D0(Proportion(0.5),point);
			return(point);
		}

		Cam::Point EndPoint() const
		{
			gp_Pnt point;
			Curve().D0(EndParameter(),point);
			return(point);
		}

		gp_Vec StartVector() const
		{
			gp_Vec vec;
			gp_Pnt point;
			Curve().D1(StartParameter(), point, vec);
			if (! m_is_forwards) vec.Reverse();
			return(vec);
		}

		gp_Vec EndVector() const
		{
			gp_Vec vec;
			gp_Pnt point;
			Curve().D1(EndParameter(), point, vec);
			if (! m_is_forwards) vec.Reverse();
			return(vec);
		}

		Standard_Real Length() const
		{
			if (m_length < 0)
			{
				m_length = GCPnts_AbscissaPoint::Length( Curve() );
			}

			return(m_length);
        }

		bool IsForwards() const { return(m_is_forwards); }
		void Reverse()
		{
			m_is_forwards = ! m_is_forwards;
		}

		// HeeksObj *Sketch();
		std::list<Cam::Point> Intersect( const Path & rhs, const bool stop_after_first_point = false ) const;
		
		Standard_Real Tolerance() const
		{
			if (m_tolerance < 0)
			{
				m_tolerance = Cam::GetTolerance();
			}

			return(m_tolerance); 
		}

		Standard_Boolean operator==( const Path & rhs ) const;
		std::pair<gp_Pnt, gp_Vec> Nearest(const gp_Pnt reference_location, Standard_Real *pParameter = NULL, const bool snap_to_nearest_curve_endpoint = true ) const;
		#ifdef HEEKSCNC
			void Align( CFixture fixture );
		#endif // HEEKSCNC

		Path Section( const Standard_Real start_distance, const Standard_Real end_distance ) const;
		BRepAdaptor_Curve & Curve() const
		{
			if (m_pCurve.get() == NULL)
			{
				m_pCurve = std::auto_ptr<BRepAdaptor_Curve>(new BRepAdaptor_Curve(m_edge));
			}

			return(*m_pCurve); 
		}

		// libArea interface
		std::list<area::CVertex> Vertices() const;
		area::CCurve AreaCurve() const;

		void Transform( const gp_Trsf transformation );
		void Split( const Cam::Paths &areas, Cam::Paths *pInside, Cam::Paths *pOutside ) const;

		QString KiCadBoardOutline(const int layer, const int trace_width) const;

	private:
		TopoDS_Edge m_edge;
		mutable std::auto_ptr<BRepAdaptor_Curve> m_pCurve;
		bool		m_is_forwards;
		mutable Standard_Real m_length;
		mutable Standard_Real m_tolerance;

	public:
		friend QString & operator<< ( QString & str, const Path & path )
		{
			str += QString::fromUtf8("<Path ");

			switch(path.Curve().GetType())
			{
			case GeomAbs_Line:
				str += QString::fromUtf8("type=\"GeomAbs_Line\"");
				break;

			case GeomAbs_Circle:
				str += QString::fromUtf8("type=\"GeomAbs_Circle\"");
				break;

			case GeomAbs_Ellipse:
				str += QString::fromUtf8("type=\"GeomAbs_Ellipse\"");
				break;

			case GeomAbs_Hyperbola:
				str += QString::fromUtf8("type=\"GeomAbs_Hyperbola\"");
				break;

			case GeomAbs_Parabola:
				str += QString::fromUtf8("type=\"GeomAbs_Parabola\"");
				break;

			case GeomAbs_BezierCurve:
				str += QString::fromUtf8("type=\"GeomAbs_BezierCurve\"");
				break;

			case GeomAbs_BSplineCurve:
				str += QString::fromUtf8("type=\"GeomAbs_BSplineCurve\"");
				break;

			case GeomAbs_OtherCurve:
				str += QString::fromUtf8("type=\"GeomAbs_OtherCurve\"");
				break;
			}

			str += _T(", direction=\"") << (wxChar *) (path.m_is_forwards?_T("FORWARDS"):_T("BACKWARDS")) << _T("\"");
			str += _T(", start_parameter=\"") << path.StartParameter() << _T("\"");
			str += _T(", end_parameter=\"") << path.EndParameter() << _T("\"");

			str += _T(", start_point=\"") << path.StartPoint().X() << _T(",") << path.StartPoint().Y() << _T(",") << path.StartPoint().Z() << _T("\"");
			str += _T(", end_point=\"") << path.EndPoint().X() << _T(",") << path.EndPoint().Y() << _T(",") << path.EndPoint().Z() << _T("\"/>\n");
			return(str);
		}

	}; // End Path class definition

	/**
		This is a container of multiple Path objects that are both ordered and contiguous (connected)
		It is similar to the Paths (plural) class except that the Paths class can hold a list of
		ContiguousPath objects.  It can also merge ContiguousPath objects if the addition of an edge
		marries two ContiguousPath objects together.
	 */
	class ContiguousPath
	{
	private:
		typedef enum {
			YZ = 1,
			XZ,
			XY
		} ePlane_t;

	public:
		ContiguousPath() { m_is_forwards = true; m_concentricity = -1; m_id = s_next_available_id++; }
		ContiguousPath( const ContiguousPath & rhs );
		ContiguousPath & operator= ( const ContiguousPath & rhs );

		bool Add(Path path);
		bool Contains(Path path) const;
		Standard_Real Length() const;
		void D0(const Standard_Real DistanceAlong, gp_Pnt &P);
		void D1(const Standard_Real DistanceAlong, gp_Pnt &P, gp_Vec &V);
		bool IsForwards() const { return(m_is_forwards); }
		void Reverse();
        bool Periodic() const;
		std::vector<Path>::iterator Next(std::vector<Path>::iterator itPath);
		Cam::Point StartPoint() const;
		Cam::Point EndPoint() const;
		bool Surrounds( const Cam::Point point ) const;
		bool Surrounds( const ContiguousPath & rhs ) const;
		TopoDS_Wire Wire() const;
		std::pair<gp_Pnt, gp_Vec> Nearest(const gp_Pnt reference_location) const;
		gp_Pln Plane() const;
		ContiguousPath Section( const Standard_Real start_distance, const Standard_Real end_distance );
		ContiguousPath InterpolateLines(const double max_deviation, const bool retain_simple_curve_types = false) const;
		gp_Pnt Centroid() const;
		std::list<Cam::Point> Intersect( const ContiguousPath & rhs, const bool stop_after_first_point = false ) const;

		ContiguousPath & operator+=( ContiguousPath &rhs );

		std::vector<Path>::iterator SetStartPoint( Cam::Point location );

		HeeksObj *Sketch(HeeksObj *sketch = NULL);
		void Reset();

		Bnd_Box BoundingBox() const;
		bool IsClockwise();
		bool operator< (const ContiguousPath & rhs ) const;	// Sort by nesting (i.e. object inside object etc.)

		int Concentricity() const { return(m_concentricity); }
		void Concentricity(const int value) { m_concentricity = value; }

		typedef unsigned int Id_t;
		typedef std::vector< Id_t > Ids_t;

		Id_t	ID() const { return(m_id); }
		Ids_t PathsThatSurroundUs() const { return(m_paths_that_surround_us); }
		void PathsThatSurroundUs(const Id_t id) const;

		Ids_t PathsThatWeSurround() const { return(m_paths_that_we_surround); }
		void PathsThatWeSurround( const Id_t id ) const;
		Cam::Point MidpointForSurroundsCheck() const;

		void Transform( const gp_Trsf transformation );

		#ifdef HEEKSCNC
			void Align( CFixture fixture );
		#endif // HEEKSCNC

		std::vector<Path> Paths();
		void Split( const Cam::Paths &areas, Cam::Paths *pInside, Cam::Paths *pOutside ) const;

		area::CCurve AreaCurve();

		QString KiCadBoardOutline(const int layer, const int trace_width) const;

	protected:
		bool m_is_forwards;
		std::vector<Path> m_paths;
		
		int m_concentricity;	// more positive the value indicates an inner (enclosed) polygon.
		Id_t	m_id;
		static Id_t s_next_available_id;
		mutable Ids_t		m_paths_that_we_surround;
		mutable Ids_t		m_paths_that_surround_us;
		mutable std::auto_ptr<gp_Pln>	m_plane;
		mutable std::auto_ptr<Bnd_Box> m_pBoundingBox;

	public:
		friend QString & operator<< ( QString & str, const ContiguousPath & path )
		{
			str << QString::fromUtf8("<ContiguousPath ") << _T(" num_paths=\"") << path.m_paths.size() << _T("\"");
			str << _T(", direction=\"") << (wxChar *) (path.m_is_forwards?_T("FORWARDS"):_T("BACKWARDS")) << _T("\"") << _T(">\n");

			for (std::vector<Path>::const_iterator itPath = path.m_paths.begin(); itPath != path.m_paths.end(); itPath++)
			{
				str << *itPath;
			}

			str << QString::fromUtf8("</ContiguousPath>\n");

			return(str);
		}
	}; // End of ContiguousPath class definition.

	class Paths
	{
	public:
		Paths() { }
		Paths(HeeksObj *object);
		Paths(const TopoDS_Shape shape);
		Paths(ContiguousPath contiguous_path);
		Paths(area::CArea &area);
		Paths( const Paths & rhs );
		Paths & operator= ( const Paths & rhs );
		Paths & operator-= ( const Paths & rhs );

		void Reverse()
		{
			m_is_forwards = ! m_is_forwards;
			for (std::vector<ContiguousPath>::iterator itPath = m_contiguous_paths.begin(); itPath != m_contiguous_paths.end(); itPath++)
			{
				itPath->Reverse();
			}
		}

		::size_t size() const { return(m_contiguous_paths.size()); }
		ContiguousPath operator[]( const int offset ) const { return(m_contiguous_paths[offset]); }
		ContiguousPath GetByID( const ContiguousPath::Id_t id ) const;

		Path Next() const;
		void Add(const Path path);
		void Add(const TopoDS_Shape shape);
		void Add(const TopoDS_Edge edge);
		void Add(const TopoDS_Wire wire, const gp_Pln reference_plane, const double maximum_distance);
		void Add(area::CArea &area);
		void Add(HeeksObj *object);
		bool Join( std::vector<ContiguousPath>::iterator lhs, std::vector<ContiguousPath>::iterator rhs );
		bool Offset(const double distance);
		#ifdef HEEKSCNC
			void Align( CFixture fixture );
		#endif // HEEKSCNC

        HeeksObj *Sketch();
		void Sort(const bool force = false);

		area::CArea Area();
		void Transform( const gp_Trsf transformation );
		Bnd_Box BoundingBox() const;

		void Split( const Cam::Paths &areas, Cam::Paths *pInside, Cam::Paths *pOutside ) const;
		std::set<int> GetConcentricities() const;
		QString KiCadBoardOutline(const int layer = 28, const int trace_width = 150) const;

	protected:
		std::vector<ContiguousPath> m_contiguous_paths;
		bool		m_is_forwards;
	}; // End Paths class definition.


	double deflection( const Cam::Point start, const Cam::Point middle, const Cam::Point end );
	std::list<Cam::Point> InterpolateCurve( BRepAdaptor_Curve curve, const Standard_Real start_u, const Standard_Real end_u, const double max_deviation );



} // End Cam namespace.


