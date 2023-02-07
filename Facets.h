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
#include "BasicStructs.h"


struct FacetPointsTriplet;
class FacetPoint;
class Facet;
class FacetBody;
class CompositeFacetBody;

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
};

class CompositeFacetBody{
	private:
		std::vector<FacetBody> facet_bodies;
		Validity valid;
		
		bool MakeFacetPointsFromTriangles(const std::vector<Triangle3D> &triangles, std::map<Point3D, FacetPoint*> &result_map);
		bool MakeFacetsFromTriangles(const std::vector<Triangle3D> &triangles, std::set<Facet*> &result_facets_vector);
		bool GroupConnectedFacetsTogether(std::set<Facet*> source_facets, std::vector<std::set<Facet*>> &result_groups);
	public:
		CompositeFacetBody(std::vector<Triangle3D> triangles);
		~CompositeFacetBody();
		void Shift(MathVector3D shift_vector);
		void Rotate(Matrix3D turn_matrix);
		std::pair<double, double> GetZExtremums() const;
		void PrintCompositeBody() const;
};

#endif