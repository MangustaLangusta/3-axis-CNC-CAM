#include "Facets.h"

std::vector<Point3D> Facets::GetCommonPoints(const Facet* facet_a, const Facet* facet_b){
	std::vector<Point3D> result_vector;
	std::vector<FacetPoint*> vector_a = facet_a->GetPoints();
	std::vector<FacetPoint*> vector_b = facet_b->GetPoints();
	for(auto &it_a : vector_a){
		for(auto &it_b : vector_b)
			if(it_a == it_b)
				result_vector.push_back(it_a->GetCoordinates());
	}
	return result_vector;
}

bool Facets::IntersectionFacetsAndZPlane(const Facet* facet_a, const Facet* facet_b, double z_plane, Point3D* result_point){
	result_point = NULL;
	std::vector<Point3D> common_points = Facets::GetCommonPoints(facet_a, facet_b);
	switch(common_points.size()){
		case 0:
			return false;
		case 1:
			if(common_points[0].z == z_plane){
				*result_point = common_points[0];
				return true;
			}
			else 
				return false;
		case 2:
			if(MathOperations::IntersectionOfLineAndZPlane(Line3D(common_points[0], common_points[1]), z_plane, result_point))
				return true;
			else 
				return false;
		default: 
			assert(false);	//should not be happened
			return false;
	}
}

FacetPoint::FacetPoint(Point3D new_coords){
	coordinates = new_coords;
	parent_facets.clear();
}

FacetPoint::~FacetPoint(){}

const Point3D FacetPoint::GetCoordinates() const { return coordinates; }

const double FacetPoint::GetX() const { return coordinates.x; }

const double FacetPoint::GetY() const { return coordinates.y; }

const double FacetPoint::GetZ() const { return coordinates.z; }

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
	double current_z = points[0]->GetZ();
	double z_max = current_z;
	double z_min = current_z;
	for(int i = 1; i < 3; i++){
		current_z = points[i]->GetZ();
		z_max = current_z > z_max ? current_z : z_max;
		z_min = current_z < z_min ? current_z : z_min;
	}
	return std::make_pair(z_min, z_max);	
}

int Facet::AmountOfIntersectionsWithZPlane(const double z_plane) const{
	const std::pair<double, double> facet_extremums = GetZExtremums();
	if( (facet_extremums.first > z_plane) || (facet_extremums.second < z_plane) )
		return 0;
	if( (facet_extremums.first < z_plane) && (facet_extremums.second > z_plane) )
		return 2;
	if( (facet_extremums.first == z_plane) && (facet_extremums.second == z_plane) )
		return 3;
	if( (facet_extremums.first == z_plane) || (facet_extremums.second == z_plane) ){
		int amounts = 0;
		for(int i = 0; i < 3; i++)
			if(points[i]->GetZ() == z_plane)
				amounts++;
		return amounts;		
	}
	assert(false);	//should never be happened
	return 0;
}

void Facet::PrintFacet() const {
	std::cout<<"* *"<<std::endl;
	std::cout<<"Facet addr: "<<this<<"   points:"<<std::endl;
	for(int i = 0; i < 3; i++)
		points[i]->PrintFacetPoint();
	std::cout<<"Normal: ";
	Print(normal.vector_top);
}

std::vector<FacetPoint*> Facet::GetPoints() const{
	std::vector<FacetPoint*> result_vector;
	for(int i = 0; i < 3; i++)
		result_vector.push_back(points[i]);
	return result_vector;
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

bool FacetBody::SplitByZPlane(const double z_plane, std::vector<std::list<Point3D>> *result_contours){
	/*
	(one Facet body potentially can be split into more than one contour by one plane)
	*/
	result_contours->clear();
	Point3D cross_point;
	std::list<Point3D> current_waypoints;
	Facet* facet_base = NULL;
	Facet* first_facet = NULL;
	Facet* previous_facet_base = NULL;
	std::set<Facet*> current_neighbours;
	
	bool found_next_facet;
	/*
	Make 2 sets of Facets "One cross point", "Two cross points"
	*/
	std::set<Facet*> one_cross_point, two_cross_points;
	/*
	For each facet in body:
		Check amount of cross points (input: contour's plane, output: Amount of cross points (0,1,2 or 3) )
			If 1 or 2 cross points, add facet in respective set
	End for
	*/
	int amounts;
	for(auto &it : facets){
		amounts = it->AmountOfIntersectionsWithZPlane(z_plane);
		if(amounts == 2)
			two_cross_points.insert(it);
		if(amounts == 1)
			one_cross_point.insert(it);		
	}
	/*
	While set "Two cross points" is not empty:
		If Facet_base is not defined (NULL):
			facet_base is first facet from set "Two cross points"	
			first_facet_base is facet_base
			if current_final_list is not empty:
				push current_final_list into result array
				create new current_final_list
			end if
		End if
		Array neighbours is all neighbours of facet_base
		previous_facet_base is facet_base
		found_next_facet is false
		For each of neighbours:
			If current_neighbour belongs to set "one cross point"
				facet_base is set to current_neighbour 
				found_next_facet is true
				break loop
			If current_neighbour belongs to set "two cross points",
				Find point of intersecton of common line (joining facet_base and current_neighbour) and plane
				Add this point to waypoints
				Facet_base is set to current_neighbour 
				found_next_facet is true
				break loop
		End for
		If found_next_facet is false:
			if first_facet_base is in current_neighbours:
				Find point of intersecton of common line (joining facet_base and first_facet_base) and plane
				Add this point to waypoints
			End if
			facet_base is not defined (NULL)
		End if
		Remove previous_facet_base from set "Two cross points"
		Remove previous_facet_base from set "One cross point" (it can be in any of these sets)
	End while
	*/
	while(!two_cross_points.empty()){
		if(facet_base == NULL){
			facet_base = *two_cross_points.begin();
			first_facet = facet_base;
			if(!current_waypoints.empty()){
				result_contours->push_back(current_waypoints);
				current_waypoints.clear();
			}
		}
		current_neighbours = facet_base->GetNeighbours();
		previous_facet_base = facet_base;
		found_next_facet = false;
		for(auto &neighbour : current_neighbours){
			if(one_cross_point.find(neighbour) != one_cross_point.end()){
				facet_base = neighbour;
				found_next_facet = true;
				break;				
			}
			if(two_cross_points.find(neighbour) != two_cross_points.end()){
				if( Facets::IntersectionFacetsAndZPlane(facet_base, neighbour, z_plane, &cross_point) )
					current_waypoints.push_back(cross_point);
				else {
					assert(false);
					return false;
				}
				facet_base = neighbour;
				found_next_facet = true;
				break;
			}
		}
		if(!found_next_facet){
			if(current_neighbours.find(first_facet) != current_neighbours.end()){
				if( Facets::IntersectionFacetsAndZPlane(facet_base, first_facet, z_plane, &cross_point) )
					current_waypoints.push_back(cross_point);
				else {
					return false;
					assert(false);
				}
			}
			facet_base = NULL;
		}
		two_cross_points.erase(two_cross_points.find(previous_facet_base));
		one_cross_point.erase(one_cross_point.find(previous_facet_base));
	}	
	/*		
	While set "One cross point" is not empty
		If Facet_base is not defined (NULL):
			if current_final_list is not empty:
				push current_final_list into result array
				create new current_final_list
			end if
			facet_base is first facet from set "One cross point"	
			Add this one cross point to wwaypoints (it's one of facet's edges)
		End if
		Array neighbours is all neighbours of facet_base
		previous_facet_base is facet_base
		found_next_facet is false
		For each of neighbours:
			If current_neighbour belongs to set "one cross point"
				facet_base is set to current_neighbour 
				found_next_facet is true
				break loop
			End if
		End for
		if found_next_facet is false:
			if first_facet is NOT in current_neighbours
				error: it should be
			facet_base is not defined (NULL)
		Remove previous_facet_base from set "One cross point"
	End while
	*/
	assert(facet_base == NULL);
	while(!one_cross_point.empty()){
		if(facet_base == NULL){
			if(!current_waypoints.empty()){
				result_contours->push_back(current_waypoints);
				current_waypoints.clear();
			}
			facet_base = *one_cross_point.begin();
			first_facet = facet_base;			
		}
		current_neighbours = facet_base->GetNeighbours();
		previous_facet_base = facet_base;
		found_next_facet = false;
		for(auto &neighbour : current_neighbours){
			if(one_cross_point.find(neighbour) != one_cross_point.end()){
				if( !Facets::IntersectionFacetsAndZPlane(facet_base, neighbour, z_plane, &cross_point) ){
					assert(false);
					return false;
				}
				facet_base = neighbour;
				found_next_facet = true;
				break;				
			}
		}
		if(!found_next_facet){
			current_waypoints.push_back(cross_point);
			if(current_neighbours.find(first_facet) == current_neighbours.end()){
				assert(false);	//It should be there
				return false;
			}
			facet_base = NULL;
		}
		one_cross_point.erase(one_cross_point.find(previous_facet_base));
	}
	return true;
}

void FacetBody::PrintBody() const {
	std::cout<<"* * * *"<<std::endl;
	std::cout<<"Facet Body: "<<std::endl;
	for(auto &it: facets)
		it->PrintFacet();
}

CompositeFacetBody::CompositeFacetBody(const std::vector<Triangle3D> triangles, std::list<Error> *errors_list, bool *error_flag){
	std::set<Facet*> all_facets;
	std::vector<std::set<Facet*>> facet_groups;
	bool abort = false;

	if(!MakeFacetsFromTriangles(triangles, all_facets)){
		*error_flag = true;
		errors_list->push_back(ERROR_FACET_BODY_UNABLE_TO_CREATE);
		return;
	}

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
		*error_flag = true;
		errors_list->push_back(ERROR_FACET_BODY_UNABLE_TO_CREATE);
		return;
	}	

	for(auto &it : facet_groups)
		facet_bodies.emplace_back(FacetBody(it));

	this->valid = VALID;
	for(auto &it : facet_bodies)
		if(!it.IsValid())
			this->valid = NOT_VALID;
	if(this->valid == NOT_VALID){
		*error_flag = true;
		errors_list->push_back(ERROR_FACET_BODY_VALIDITY_CHECK_NOT_PASSED);
	}
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