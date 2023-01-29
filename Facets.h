#ifndef FACETS_H
#define FACETS_H

class Facet;

class FacetPoint{
	private:
		Point3D coordinates;
		std::vector<Facet*> parent_facets;
		
		friend bool operator< (const FacetPoint &facet_point_a, const FacetPoint &facet_point_b);
		friend bool operator== (const FacetPoint &facet_point_a, const FacetPoint &facet_point_b);
	public:
		Point3D GetCoordinates();
	
	
};

class Facet{
	private:
		FacetPoint* points[3];
		MathVector3D normal;
		std::vector<Facet*> neighbour_facets;
	public:
		Facet(const STLTriangle &stl_triangle){
			
			
		}
};



class FacetBody{
	private:
		std::vector<Facet*> facets;
		Validity valid;
	public:
	
};

class CompositeFacetBody{
	private:
		std::vector<FacetBody> facet_bodies;
	
	public:
		CompositeFacetBody(std::vector<Triangle3D> triangles){
			std::set<Point3D> new_body_points;
			std::vector<FacetPoint*> new_facet_points;
			std::vector<Facet*> new_facets;
			/*	Make first pass through triangles - create non-repetitive set of all points		*/
			for(auto &it_triangle : triangles){
				for(int i = 0; i < 3; i++)
					new_body_points.insert(it_triangle.vertex[i]);
			}
			
			// OK TILL THIS POINT 
			
				
			
			
			
			/*	Second pass: make facets	*//*
			for(auto &it_triangle : triangles){
				
				
			}
			*/
			
		}
		
		
};

Point3D FacetPoint::GetCoordinates() { return coordinates; }

bool operator< (const FacetPoint &facet_point_a, const FacetPoint &facet_point_b){
	Point3D a = facet_point_a.coordinates;
	Point3D b = facet_point_b.coordinates;
	if(a.z < b.z)
		return true;
	if(a.y < b.y)
		return true;
	if(a.x < b.x)
		return true;
	return false;
}

bool operator== (const FacetPoint &facet_point_a, const FacetPoint &facet_point_b){
	Point3D a = facet_point_a.coordinates;
	Point3D b = facet_point_b.coordinates;
	if((a.z == b.z)&&(a.y == b.y)&&(a.x == b.x))
		return true;
	return false;
}


#endif