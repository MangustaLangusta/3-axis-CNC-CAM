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
	assert(result_point != NULL);
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

bool FacetPoint::IsParent(Facet* parent_candidate) const {
	return (parent_facets.find(parent_candidate) != parent_facets.end());
}

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

std::vector<Line3D> Facet::GetBordersAsLines() const {
	std::vector<Line3D> result_vector;
	for(int i = 0; i < 3; i++)
		result_vector.push_back( {points[i]->GetCoordinates(), points[(i+1) % 3]->GetCoordinates()} );
	return result_vector;
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

bool Facet::IsSuitsForContour(const double &z_plane) const {
	std::pair<double, double> z_extremums;
		//z_extremums.first chould be z_min,   z_extremums.second should be z_max
	z_extremums = GetZExtremums();
		//if facet fully lays in z_plane, also not suits 
	return( (z_plane >= z_extremums.first) && (z_plane <= z_extremums.second) && (z_extremums.first != z_extremums.second) );
}

Facet* Facet::GetNextFacetForContour(const double &z_plane) const {
	/*
		Waypoints order in contour: cut (empty) area always from the right side if pass waypoints in forward direction
																solid (body) area always left side (watching from the top)
		Other words, contour direction through facet body surface is anti-clockwise if to see from top
	*/
	const int SUITABLE_NEIGHBOUR_A = 0;
	const int SUITABLE_NEIGHBOUR_B = 1;
	
	std::vector<Facet*> suitable_neighbours;
	FacetPoint* common_points[2];
	
		//check this contour first for safety
	assert(this->IsSuitsForContour(z_plane));
	
		//find 2 of ajacent neighbour facets, suitable for contour (in any case, it will be 2 facets)
	for(auto &it : GetNeighbours()){
		if(it->IsSuitsForContour(z_plane))
			suitable_neighbours.push_back(it);
	}
	assert(suitable_neighbours.size() == 2);
	
		//point, which belongs to both suitable neighbours, is ignored
		//two other points belongs only to corresponding neighbour. find these points
	for(auto &it : GetPoints()){
		if(it->IsParent(suitable_neighbours[SUITABLE_NEIGHBOUR_A]) && it->IsParent(suitable_neighbours[SUITABLE_NEIGHBOUR_B]))
			continue;
		if( it->IsParent(suitable_neighbours[SUITABLE_NEIGHBOUR_A]) ){
			common_points[SUITABLE_NEIGHBOUR_A] = it;
			continue;
		}
		if( it->IsParent(suitable_neighbours[SUITABLE_NEIGHBOUR_B]) ){
			common_points[SUITABLE_NEIGHBOUR_B] = it;
			continue;
		}
		assert(false);
	}
	
		//vector from one edge point to another
	MathVector3D direct_vector( common_points[SUITABLE_NEIGHBOUR_A]->GetCoordinates(), common_points[SUITABLE_NEIGHBOUR_B]->GetCoordinates() );
		//vector multiplication of this facet's normal and our neighbours vector
	MathVector3D vector_mult_result = MathOperations::VectorMultiplication(normal, direct_vector);
		//z == 0 is possible only if both normal and direct vector lays perpendicular to z_plane, 
		//but that would be a mistake in previous parts of program
	assert(vector_mult_result.GetZ() != 0);
		//if z sign of result vector is positive, direction is correct. Otherwise, opposite direction of contour
	if(vector_mult_result.GetZ() > 0)
		return suitable_neighbours[SUITABLE_NEIGHBOUR_B];
	else 
		return suitable_neighbours[SUITABLE_NEIGHBOUR_A];	
}

Point3D Facet::GetNextContourPoint(const double &z_plane) const {
	//find point of facet on z_plane and one which is second of two points in contour (or the only one point, if facet crosses z_plane in only one point)
	//It's guaranteed, that facet is not belongs to z_plane, but crosses it (one or two points of contact)
	
	const int POINT_A_INDEX = 0;
	const int POINT_B_INDEX = 1;
	Point3D line_point_a, line_point_b, tmp_point;
	std::vector<Point3D> candidate_points;
	std::vector<Line3D> candidate_lines;
	
		//check each line of facet and find intersection points
	candidate_lines = GetBordersAsLines();
	for(auto &it_lines : candidate_lines){
		line_point_a = it_lines.a;
		line_point_b = it_lines.b;
			//lines, laying on z_plane, to be ignored intentionally
		if( (line_point_a.z == z_plane) && (line_point_b.z == z_plane) )
			continue;
			//two lines should cross z plane
		if(MathOperations::IntersectionOfLineAndZPlane(it_lines, z_plane, &tmp_point))
			candidate_points.push_back(tmp_point);
	}
		//double check that indeed 2 cross points
	assert(candidate_points.size() == 2);
	
		//if two points are same, in means that facet touches z_plane by only one of its vertexes
	if(candidate_points[POINT_A_INDEX] == candidate_points[POINT_B_INDEX])
		return candidate_points[POINT_A_INDEX];
		
		//Make math vector from these two points of facet
	MathVector3D points_vector( candidate_points[POINT_A_INDEX], candidate_points[POINT_B_INDEX] );
		//vector multiplication of this facet's normal and our points vector
	MathVector3D vector_mult_result = MathOperations::VectorMultiplication(normal, points_vector);
	assert(vector_mult_result.GetZ() != 0);
		//if z_coord of vectorr multiplication > 0, direction was chosen correct, can return second point
		//otherwise, return first point
	if(vector_mult_result.GetZ() > 0)
		return candidate_points[POINT_B_INDEX];
	return candidate_points[POINT_A_INDEX];	
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

bool FacetBody::SplitByZPlane(const Plane3D &plane, std::vector<std::list<Point3D>> *result_contours) const{
	bool facet_body_suits_for_contour;
	bool contour_not_finished;
	double z_plane = plane.plane_point.z;
	Facet* current_facet;
	std::set<Facet*> used_facets;
	std::list<Point3D> current_contour;
	Point3D candidate_point, last_added_point;
	
		//Each cycle corresponds to one contour. One FacetBody may have more then one contour
	while(used_facets.size() != facets.size()){
		
			//Go through all facets, which are not checked yet.
		facet_body_suits_for_contour = false;
		for(auto &it : facets){
			if(used_facets.find(it) != used_facets.end())
				continue;
				//Not forget to put all checktd facets in set to avoid checking them for next time
			used_facets.insert(it);
				//If one of them suits for contour (crosses z_plane), use this as fist facet in contour
			if(it->IsSuitsForContour(z_plane)){
				current_facet = it;
				facet_body_suits_for_contour = true;
				break;
			}
		}
		
			//if not found suitable not checked facet, break cycle - all possible contours found
		if(!facet_body_suits_for_contour)
			break;
		
			//Add waypoints to contour one by one until loop is finished
		contour_not_finished = true;
		while(contour_not_finished){
			candidate_point = current_facet->GetNextContourPoint(z_plane);
				//check that points are not duplicated
			if(!current_contour.empty()){
				if(candidate_point != last_added_point){
					current_contour.push_back(candidate_point);
					last_added_point = candidate_point;
				}
			}
			else{
				current_contour.push_back(candidate_point);
				last_added_point = candidate_point;
			}
			used_facets.insert(current_facet);
			current_facet = current_facet->GetNextFacetForContour(z_plane);
				//should not be open loops at all
			if(current_facet == NULL)
				return false;
				//when next facet already checked, it means that contour is closed.
			if(used_facets.find(current_facet) != used_facets.end())
				contour_not_finished = false;
		}
		
			//check additionally first and last points of contour - if they are same, remove last one
		if(*current_contour.begin() == *current_contour.rbegin())
			current_contour.pop_back();
			
			//contour is closed, add to contours vector
		result_contours->push_back(current_contour);
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

CompositeFacetBody::~CompositeFacetBody(){
	std::cout<<"CompositeFacetBody deleted!"<<std::endl;
}

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

std::vector<std::list<Point3D>> CompositeFacetBody::SplitByZPlane(const Plane3D z_plane, std::list<Error> *errors_list, ErrorFlag *error_flag){
	std::vector<std::list<Point3D>> result, intermediate_results;
	
	for(auto &it_bodies : facet_bodies){
		intermediate_results.clear();
		if( it_bodies.SplitByZPlane(z_plane, &intermediate_results) ){
			//!!!!!!!!!!!!!!!!!!!!!
			for(auto &it : intermediate_results)
				result.push_back(it);
		}
		else {
			error_flag->RiseError();
			errors_list->push_back(ERROR_UNABLE_TO_SPLIT_FACET_BODY_TO_CONTOURS);
			return result;
		}			
	}
	return result;
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

std::vector<std::list<Point3D>> CompositeFacetBody::SplitByZPlanes(	const std::vector<Plane3D> split_planes, 
																																		std::list<Error> *errors_list, 
																																		ErrorFlag *error_flag ) {
	
	std::vector<std::list<Point3D>> result, intermediate_results;
	
	assert(error_flag != NULL);
	assert(errors_list != NULL);
	errors_list->clear();
	for(auto &it_planes : split_planes){
		intermediate_results = SplitByZPlane(it_planes, errors_list, error_flag);
		if(!error_flag->HaveErrors()){
			std::cout<<"intermediate size = "<<intermediate_results.size()<<std::endl;
			for(auto &it : intermediate_results)
				result.push_back(it);
		}
		else{
			return result;
		}
	}
	return result;
}

void CompositeFacetBody::PrintCompositeBody() const {
	std::cout<<"*	 *  *  *  *  *  *  *  *"<<std::endl;
	std::cout<<"CompositeFacetBody: "<<std::endl;
	for(auto &it : facet_bodies)
		it.PrintBody();	
}