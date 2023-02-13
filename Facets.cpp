#include "Facets.h"

IntersectionObject::IntersectionObject(){}

IntersectionObject::~IntersectionObject(){}

IntersectionPoint::IntersectionPoint(const Point3D point) : IntersectionObject(){
	intersection_point = point;
}

IntersectionPoint::~IntersectionPoint(){}

Point3D IntersectionPoint::GetIntersectionPoint()const{
	return intersection_point;
}

IntersectionLine::IntersectionLine(const Line3D line) : IntersectionObject(){
	intersection_line = line;
}

IntersectionLine::~IntersectionLine(){}

Line3D IntersectionLine::GetIntersectionLine()const{
	return intersection_line;
}

IntersectionTriangle::IntersectionTriangle(const Triangle3D triangle) : IntersectionObject(){
	intersection_triangle = triangle;
}

IntersectionTriangle::~IntersectionTriangle(){}

Triangle3D IntersectionTriangle::GetIntersectionTriangle()const{
	return intersection_triangle;
}
		
FacetIntersection::FacetIntersection(const FacetPointsTriplet &triplet, const double z_plane){
	Point3D a = *triplet.facet_point[0];
	Point3D b = *triplet.facet_point[1];
	Point3D c = *triplet.facet_point[2];
	IntersectionObject* new_intersection_object;
	if((a.z == b.z)&&(a.z == c.z)){
		//whole facet lays in z_plane 
		new_intersection_object = new IntersectionTriangle(Triangle3D({a,b,c}));
		specifier = Triangle;
		return;
	}
}

FacetIntersection::~FacetIntersection(){
	delete intersection_object;
}

FacetIntersectionSpecifier FacetIntersection::GetSpecifier()const{
	return specifier;
}

IntersectionObject* FacetIntersection::GetIntersectionObject()const{
	return intersection_object;
}

FacetPoint::FacetPoint(Point3D new_coords){
	coordinates = new_coords;
	parent_facets.clear();
}

FacetPoint::~FacetPoint(){}

const Point3D FacetPoint::GetCoordinates() const { return coordinates; }

void FacetPoint::AddParentFacet(Facet* parent_facet_to_add) { parent_facets.insert(parent_facet_to_add); } 

std::set<Facet*> FacetPoint::GetParentFacets() const { return parent_facets; };

void FacetPoint::PrintFacetPoint() const {
	std::cout<<"facet point addr: "<<this<<" coords: ";
	Print(coordinates);
}

Facet::Facet(FacetPointsTriplet facet_points_triplet, MathVector3D new_normal){
	normal = new_normal;
	valid = NOT_VALID;
	/*Adding points and adding this facet to all FacetPoints as parent_facet; all neighbour_facets cleared	*/
	for(int i = 0; i < 3; i++){
		points[i] = facet_points_triplet.facet_point[i];
		points[i]->AddParentFacet(this);
		neighbour_facets[i] = NULL;
	}
}

Facet::~Facet(){
	for(int i = 0; i < 3; i++)
		delete points[i];
}

bool Facet::CheckNeighbourFacets(){
	std::set<Facet*> candidates_current, candidates_next;
	bool found_neighbour;
	for(int i = 0; i < 3; i++){
		found_neighbour = false;
		candidates_current = points[i]->GetParentFacets();
		candidates_next = points[(i+1)%3]->GetParentFacets();
		for(auto &it_current : candidates_current){
			if(it_current == this)
				continue;
			for(auto &it_next : candidates_next){
				if(it_current == it_next){
					neighbour_facets[i] = it_current;
					found_neighbour = true;
					break;					
				}
			}
		}
		if(!found_neighbour)
			return false;
	}
	return true;
}

bool Facet::IsValid(){
	valid = VALID;
	if( (points[0] == points[1]) ||	(points[1] == points[2]) ||	(points[2] == points[3]) ) 
		valid = NOT_VALID;
	for(int i = 0; i < 3; i++)
		if(neighbour_facets[i] == NULL)
			valid = NOT_VALID;
	if(valid == NOT_VALID)
		return false;
	return true;	
}

std::set<Facet*> Facet::GetNeighbours() const{
	std::set<Facet*> result_set;
	for(int i = 0; i < 3; i++)
		result_set.insert(neighbour_facets[i]);
	return result_set;
}

std::pair<double, double> Facet::GetZExtremums() const{
	Point3D current_coordinates = points[0]->GetCoordinates();
	double z_max = current_coordinates.z;
	double z_min = current_coordinates.z;
	for(int i = 1; i < 3; i++){
		current_coordinates = points[i]->GetCoordinates();
		z_max = current_coordinates.z > z_max ? current_coordinates.z : z_max;
		z_min = current_coordinates.z < z_min ? current_coordinates.z : z_min;
	}
	return std::make_pair(z_min, z_max);	
}

void Facet::PrintFacet() const {
	std::cout<<"* *"<<std::endl;
	std::cout<<"Facet addr: "<<this<<"   points:"<<std::endl;
	for(int i = 0; i < 3; i++)
		points[i]->PrintFacetPoint();
	std::cout<<"Normal: ";
	Print(normal.vector_top);
}

//if to see from beginning to end of line, facet normal will be pointed to right
FacetIntersection Facet::IntersectionWithZPlane(const double z) const{
	const FacetPointsTriplet triplet = {points[0], points[1], points[2]};
	return FacetIntersection(triplet, z);
}

FacetBody::FacetBody(std::set<Facet*> body_facets){
	facets = body_facets;
	if(CheckIntegrity())
		valid = VALID;
	else 
		valid = NOT_VALID;
}

FacetBody::~FacetBody(){}

bool FacetBody::CheckIntegrity() const{
	std::set<Facet*> set_to_check;
	std::set<Facet*> neighbours;
	if(facets.empty()){
		std::cout<<"FacetBody is empty!"<<std::endl;
		return false;
	}
	for(auto &it : facets){
		neighbours = it->GetNeighbours();
		for(auto &it_neighbours : neighbours)
			set_to_check.insert(it_neighbours);
	}
	if(set_to_check == facets)
		return true;
	std::cout<<"Integrity check failed!"<<std::endl;
	return false;
}

bool FacetBody::IsValid() const{ 
	if(valid == VALID)
		return true;
	return false;
}

std::pair<double, double> FacetBody::GetZExtremums() const{
	if(facets.empty())
		return std::make_pair(0,0);
	std::pair<double, double>	facet_extremums =(*facets.begin())->GetZExtremums();
	double z_min = facet_extremums.first;
	double z_max = facet_extremums.second;
	for(auto &it : facets){
		facet_extremums = it->GetZExtremums();
		z_min = facet_extremums.first < z_min ? facet_extremums.first : z_min;
		z_max = facet_extremums.second > z_max ? facet_extremums.second : z_max;
	}
	return std::make_pair(z_min, z_max);
}

std::list<Point3D> FacetBody::SplitByZPlane(double z_plane){
	std::pair<double, double> facet_extremums;
	std::set<Facet*> affected_facets;
	for(auto &it : facets){
		facet_extremums = it->GetZExtremums()
		if( (facet_extremums.first > z_plane) || (facet_extremums.second < z_plane) ){
			//z_plane below or above facet - NOT OK, it's not affected at all
			continue;
		}
		if( (facet_extremums.first < z_plane) && (facet_extremums.second > z_plane) ){
			//z_plane between facet extremums - OK, this will form part of contour
			affected_facets.insert(it);
			continue;
		}
		if( (facet_extremums.first == z_plane) && (facet_extremums.second == z_plane) ){
			//both max_z and min_z in z_plane -- NOT OK, whole facet in z_plane, we will use neighbourhoods instead
			continue;
		}
		if( (facet_extremums.first == z_plane) || (facet_extremums.second == z_plane) ){
			//
		}
	}
	/*
	Make 2 sets of Facets "One cross point", "Two cross points"
	For each facet in body:
		Check amount of cross points (input: contour's plane, output: Amount of cross points (0,1,2 or 3) )
			If 1 or 2 cross points, add facet in respective set
	End for
	If at least one set is not empty
		Make array of lists of Facets "result contours" (one Facet body potentially can be split into more than one contour by one plane)
	Take any facet_base from set "Two cross points"
	Facet_base is also First_facet_base
	Take first list of Facets from result_contours - "current_final_list"
	While set "Two cross points" is not empty:
		For each neighbour of Facet_base:
			If neighbour belongs to set "one cross point"
				
				remove neighbour from set "one cross point"
			If neighbour belongs to set "two cross points",
				Find point of intersecton of common line and plane
				Add this point to current final list
				Remove Facet_base from set "Two cross points"
				Neighbour is next Facet_base now
		End for
		Remove Facet_base from set "Two cross points"
		
	*/
}

void FacetBody::PrintBody() const {
	std::cout<<"* * * *"<<std::endl;
	std::cout<<"Facet Body: "<<std::endl;
	for(auto &it: facets)
		it->PrintFacet();
}

CompositeFacetBody::CompositeFacetBody(std::vector<Triangle3D> triangles, std::list<Error> *new_errors_list){
	errors_list = new_errors_list;
	std::set<Facet*> all_facets;
	std::vector<std::set<Facet*>> facet_groups;
	bool abort = false;

	if(!MakeFacetsFromTriangles(triangles, all_facets))
		return;

	for(auto &it : all_facets)
		if(!it->CheckNeighbourFacets())
			abort = true;
		
	if(!abort)
		if(!GroupConnectedFacetsTogether(all_facets, facet_groups))
			abort = true;

	if(abort){
		std::cout<<"Making composite facet body is aborted!"<<std::endl;
		for(auto &it : all_facets)
			delete it;
		return;
	}	

	for(auto &it : facet_groups)
		facet_bodies.emplace_back(FacetBody(it));

	this->valid = VALID;
	for(auto &it : facet_bodies)
		if(!it.IsValid())
			this->valid = NOT_VALID;

	std::cout<<"CompositeFacetBody successfully made"<<std::endl;
	PrintCompositeBody();
}

CompositeFacetBody::~CompositeFacetBody(){}

bool CompositeFacetBody::MakeFacetPointsFromTriangles(const std::vector<Triangle3D> &triangles, std::map<Point3D, FacetPoint*> &result_map){
	FacetPoint *ptr_facet_point = NULL;
	for(auto &it_triangle : triangles){
		for(int i = 0; i < 3; i++){
			if(result_map.find(it_triangle.vertex[i]) == result_map.end()){
				ptr_facet_point = new FacetPoint(it_triangle.vertex[i]);
				result_map.emplace(std::make_pair(it_triangle.vertex[i], ptr_facet_point));
			}			
		}
	}
	return true;
}

bool CompositeFacetBody::MakeFacetsFromTriangles(const std::vector<Triangle3D> &triangles, std::set<Facet*> &result_facets_vector){
	std::map<Point3D,FacetPoint*> facet_points_map;
	FacetPointsTriplet facet_points_triplet;
	bool abort = false;
	/*	Create FacetPoints. No info about parent facet added at this stage - only coordinates		*/
	if(!MakeFacetPointsFromTriangles(triangles, facet_points_map))
		return false;
	
	if(facet_points_map.empty())
		return false;
	
	auto it_points_map = facet_points_map.begin();
	/*	Make Facets from triangles. No neighbour facets information added at this stage	*/
	for(auto &it_triangle : triangles){
		for(int i = 0; i < 3; i++){
			it_points_map = facet_points_map.find(it_triangle.vertex[i]);
			if(it_points_map == facet_points_map.end()){
				abort = true;
				break;
			}
			facet_points_triplet.facet_point[i] = it_points_map->second;
		}		
		/*	Constructing facets. Each FacetPoint will add this new facet in parent_facet set. This will be done in Facet constructor		*/
		result_facets_vector.insert( new Facet(facet_points_triplet, it_triangle.GetNormal()) );
		if(abort)
			break;
	}
	/*	if operation was aborted, delete all newly created facets and facet points which still not deleted*/
	if(abort){
		for(auto &it : result_facets_vector)
			delete it;
		for(auto &it : facet_points_map)
			delete it.second;
		return false;
	}
	return true;
}

bool CompositeFacetBody::GroupConnectedFacetsTogether(std::set<Facet*> source_facets, std::vector<std::set<Facet*>> &result_groups){
	std::set<Facet*> current_group;
	std::set<Facet*> neighbours;
	std::list<Facet*> facets_to_check;
	std::list<Facet*>::iterator it_checking;
	std::set<Facet*> not_processed_facets;
	
	for(auto &it : source_facets)
		not_processed_facets.insert(it);
	
	while(!not_processed_facets.empty()){
		current_group.clear();
		facets_to_check.push_back(*not_processed_facets.begin());
		not_processed_facets.erase(not_processed_facets.begin());				
		while(!facets_to_check.empty()){
			it_checking = facets_to_check.begin();
			if(current_group.find(*it_checking) != current_group.end()){
				facets_to_check.pop_front();
				continue;
			}
			neighbours = (*it_checking)->GetNeighbours();
			for(auto it : neighbours){
				if(current_group.find(it) == current_group.end()){
					facets_to_check.push_back(it);
					not_processed_facets.erase(it);
				}
			}	
			current_group.insert(*it_checking);
			not_processed_facets.erase(*it_checking);
			facets_to_check.pop_front();
		}		
		result_groups.push_back(current_group);
	}
	std::cout<<"Facets groupped into "<<result_groups.size()<<" groups"<<std::endl;
	return true;
}



void CompositeFacetBody::Shift(MathVector3D shift_vector){
	std::cout<<"Some code for shifting"<<std::endl;
}

void CompositeFacetBody::Rotate(Matrix3D turn_matrix){
	std::cout<<"Some code for rotation"<<std::endl;
}

std::pair<double, double> CompositeFacetBody::GetZExtremums() const {
	if(facet_bodies.empty())
		return std::make_pair(0,0);
	std::pair<double, double> body_extremums = facet_bodies.begin()->GetZExtremums();
	double z_min = body_extremums.first;
	double z_max = body_extremums.second;
	for(auto &it : facet_bodies){
		body_extremums = it.GetZExtremums();
		z_min = body_extremums.first < z_min ? body_extremums.first : z_min;
		z_max = body_extremums.second > z_max ? body_extremums.second : z_max;
	}
	return std::make_pair(z_min, z_max);
}

void CompositeFacetBody::PrintCompositeBody() const {
	std::cout<<"*	 *  *  *  *  *  *  *  *"<<std::endl;
	std::cout<<"CompositeFacetBody: "<<std::endl;
	for(auto &it : facet_bodies)
		it.PrintBody();	
}