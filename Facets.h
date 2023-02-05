#ifndef FACETS_H
#define FACETS_H

class FacetPoint;
class Facet;

struct FacetPointsTriplet{
	FacetPoint* facet_point[3];	
};

class FacetPoint{
	private:
		Point3D coordinates;
		std::set<Facet*> parent_facets;
	public:
		FacetPoint(Point3D);
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
		void Shift(MathVector3D shift_vector);
		void Rotate(Matrix3D turn_matrix);
		std::pair<double, double> GetZExtremums() const;
		void PrintCompositeBody() const;
};

FacetPoint::FacetPoint(Point3D new_coords){
	coordinates = new_coords;
	parent_facets.clear();
}

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

FacetBody::FacetBody(std::set<Facet*> body_facets){
	facets = body_facets;
	if(CheckIntegrity())
		valid = VALID;
	else 
		valid = NOT_VALID;
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

void FacetBody::PrintBody() const {
	std::cout<<"* * * *"<<std::endl;
	std::cout<<"Facet Body: "<<std::endl;
	for(auto &it: facets)
		it->PrintFacet();
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

CompositeFacetBody::CompositeFacetBody(std::vector<Triangle3D> triangles){
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


#endif