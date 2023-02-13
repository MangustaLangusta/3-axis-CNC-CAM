#ifndef FACETS_H
#define FACETS_H

#include <iostream>
#include <set>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <cmath>
#include <algorithm>
#include "Utility.h"
#include "Contours.h"
#include "BasicStructs.h"
#include "Errors.h"


struct FacetPointsTriplet;
class FacetPoint;
class Facet;
class FacetBody;
class CompositeFacetBody;

enum FacetIntersectionSpecifier {NoIntersection, Point, Line, Triangle};

struct FacetPointsTriplet{
	FacetPoint* facet_point[3];	
};

class IntersectionObject{
	public:
		IntersectionObject();
		~IntersectionObject();
};

class IntersectionPoint : private IntersectionObject{
	private:
		Point3D intersection_point;
	public:
		IntersectionPoint(const Point3D point);
		~IntersectionPoint();
		Point3D GetIntersectionPoint()const;
};

class IntersectionLine : private IntersectionObject{
	private:
		Line3D intersection_line;
	public:
		IntersectionLine(const Line3D line);
		~IntersectionLine();
		Line3D GetIntersectionLine()const;
};

class IntersectionTriangle : private IntersectionObject{
	private:
		Triangle3D intersection_triangle;
	public:
		IntersectionTriangle(const Triangle3D triangle);
		~IntersectionTriangle();
		Triangle3D GetIntersectionTriangle()const;
};

class FacetIntersection{
	private:
		FacetIntersectionSpecifier specifier;
		IntersectionObject* intersection_object;
	public:
		FacetIntersection(const FacetPointsTriplet &triplet, const double z_plane);
		~FacetIntersection();
		FacetIntersectionSpecifier GetSpecifier()const;
		IntersectionObject* GetIntersectionObject()const;
};

class FacetPoint{
	private:
		Point3D coordinates;
		std::set<Facet*> parent_facets;
	public:
		FacetPoint(Point3D);
		~FacetPoint();
		void AddParentFacet(Facet* parent_facet_to_add);
		std::set<Facet*> GetParentFacets() const;
		const Point3D GetCoordinates() const;
		void PrintFacetPoint() const;
	
};

class Facet{
	private:
		FacetPoint* points[3];
		MathVector3D normal;
		Facet* neighbour_facets[3];
		Validity valid;
	public:
		Facet(FacetPointsTriplet facet_points_triplet, const MathVector3D new_normal);
		~Facet();
		bool IsValid();
		bool CheckNeighbourFacets();
		std::set<Facet*> GetNeighbours() const;
		void PrintFacet() const;
		std::pair<double, double> GetZExtremums() const;
		FacetIntersection IntersectionWithZPlane(const double z) const;			//if to see from beginning to end of line, facet normal will be pointed to right
};


class FacetBody{
	private:
		std::set<Facet*> facets;
		Validity valid;
		bool CheckIntegrity() const;
	public:
		FacetBody(std::set<Facet*> body_facets);
		~FacetBody();
		bool IsValid() const;
		void PrintBody() const;
		std::pair<double, double> GetZExtremums() const;
		std::list<Point3D> SplitByZPlane(double z_plane);
		//std::multimap<double, Contour> FindContoursByZPlanes(const std::set<double> z_palnes) const;
		
};

class CompositeFacetBody{
	private:
		std::vector<FacetBody> facet_bodies;
		Validity valid;
		std::list<Error> *errors_list;
		bool MakeFacetPointsFromTriangles(const std::vector<Triangle3D> &triangles, std::map<Point3D, FacetPoint*> &result_map);
		bool MakeFacetsFromTriangles(const std::vector<Triangle3D> &triangles, std::set<Facet*> &result_facets_vector);
		bool GroupConnectedFacetsTogether(std::set<Facet*> source_facets, std::vector<std::set<Facet*>> &result_groups);
	public:
		CompositeFacetBody(std::vector<Triangle3D> triangles, std::list<Error> *new_errors_list);
		~CompositeFacetBody();
		void Shift(MathVector3D shift_vector);
		void Rotate(Matrix3D turn_matrix);
		std::pair<double, double> GetZExtremums() const;
		//std::multimap<double, Contour> FindContoursByZPlanes(const std::set<double> z_palnes) const;
		void PrintCompositeBody() const;
};

#endif