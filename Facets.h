#ifndef FACETS_H
#define FACETS_H

#include <iostream>
#include <set>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <assert.h>
#include "Utility.h"
#include "Contours.h"
#include "BasicStructs.h"
#include "Errors.h"


struct FacetPointsTriplet;
class FacetPoint;
class Facet;
class FacetBody;
class CompositeFacetBody;

namespace Facets {
	std::vector<Point3D> GetCommonPoints(const Facet* facet_a, const Facet* facet_b);
	bool IntersectionFacetsAndZPlane(const Facet* facet_a, const Facet* Facet_b, double z_plane, Point3D* result_point);
};

struct FacetPointsTriplet{
	FacetPoint* facet_point[3];	
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
		bool IsParent(Facet* parent_candidate) const;
		const Point3D GetCoordinates() const;
		const double GetX() const;
		const double GetY() const;
		const double GetZ() const;
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
		std::vector<FacetPoint*> GetPoints() const;
		std::vector<Line3D> GetBordersAsLines() const;
		void PrintFacet() const;
		std::pair<double, double> GetZExtremums() const;
		bool IsSuitsForContour(const double &z_plane) const;
		Facet* GetNextFacetForContour(const double &z_plane) const;
		Point3D GetNextContourPoint(const double &z_plane) const;
	
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
		bool SplitByZPlane(const Plane3D &plane, std::vector<std::list<Point3D>> *result_contours) const;
};

class CompositeFacetBody{
	private:
		std::vector<FacetBody> facet_bodies;
		Validity valid;
		bool MakeFacetPointsFromTriangles(const std::vector<Triangle3D> &triangles, std::map<Point3D, FacetPoint*> &result_map);
		bool MakeFacetsFromTriangles(const std::vector<Triangle3D> &triangles, std::set<Facet*> &result_facets_vector);
		bool GroupConnectedFacetsTogether(std::set<Facet*> source_facets, std::vector<std::set<Facet*>> &result_groups);
		std::vector<std::list<Point3D>> SplitByZPlane(const Plane3D z_plane, std::list<Error> *errors_list, ErrorFlag *error_flag);
	public:
		CompositeFacetBody(const std::vector<Triangle3D> triangles, std::list<Error> *errors_list, bool *error_flag);	//if have errors, flag error = true; 
		~CompositeFacetBody();
		void Shift(MathVector3D shift_vector);
		void Rotate(Matrix3D turn_matrix);
		std::pair<double, double> GetZExtremums() const;
		std::vector<std::list<Point3D>> SplitByZPlanes(const std::vector<Plane3D> split_planes, std::list<Error> *errors_list, ErrorFlag *error_flag);
		void PrintCompositeBody() const;
};

#endif