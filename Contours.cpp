#include "Contours.h"

Contour::Contour(const std::list<Point3D> &new_waypoints, ErrorsLog* errors_log){
	if(new_waypoints.size() == 0){
		valid = false;
		errors_log->AddError(ERROR_CONTOUR_NOT_VALID);
		return;
	}
		//check that all waypoints are in same plane
	z_plane = new_waypoints.begin()->z;
	for(auto &it : new_waypoints){
		if(it.z != z_plane){
			valid = false;
			errors_log->AddError(ERROR_CONTOUR_NOT_VALID);
			return;
		}
	}
		//everything fine, contour valid
	valid = true;
	waypoints = new_waypoints;
		//remove inner waypoints from stright lines
	RemoveExessivePoints();
}

Contour::Contour(const std::list<Point3D> &new_waypoints, std::set<Contour*> *contours_aparted, ErrorsLog* errors_log){
	std::cout<<"Inside constructor for self-crossing check of contours"<<std::endl;
}

Contour::~Contour(){}

void Contour::RemoveExessivePoints(){
	std::list<Point3D>::iterator wpt_before, wpt_after, wpt_inner;
	MathVector3D line_short, line_long;
	int wpts_origin_size;
	
		//no need to do anything for contours consisting of 3 wpts or less
	if(waypoints.size() < 4)
		return;
		
	wpt_before = wpt_inner = wpt_after = waypoints.begin();
	wpt_inner++;
	wpt_after++;
	wpt_after++;
	
	wpts_origin_size = waypoints.size();
	for(int i = 0; i <= wpts_origin_size; i++){
		line_short = MathVector3D(*wpt_before, *wpt_inner);
		line_long = MathVector3D(*wpt_before, *wpt_after);
		
		if(MathOperations::AngleBetweenVectors(line_short, line_long) == 0){
			waypoints.erase(wpt_inner);
			wpt_inner = wpt_after;
			wpt_after++;
		}
		else{
			wpt_before++;
			wpt_inner++;
			wpt_after++;
		}
			
		if(wpt_before == waypoints.end())
			wpt_before = waypoints.begin();
		if(wpt_inner == waypoints.end())
			wpt_inner = waypoints.begin();
		if(wpt_after == waypoints.end())
			wpt_after = waypoints.begin();
		
		if(waypoints.size() < 4)
			break;
	}
	
}

double Contour::GetZPlane() const {
	return z_plane;
}

std::list<Point3D> Contour::GetWaypoints(ErrorsLog *errors_log) const {
	if(!IsValid()){
		errors_log->AddError(ERROR_CONTOUR_NOT_VALID);
		return std::list<Point3D> {};
	}
	return waypoints;
}

bool Contour::IsValid() const {
	return valid;
}

void Contour::PrintContour() const{
	for(auto &it : waypoints)
		Print(it);
}

WorkField::WorkField(const std::pair<double, double> &new_z_extremums, Contour* new_base_contour, ErrorsLog *errors_log){
	double z_min = std::min(new_z_extremums.first, new_z_extremums.second);
	double z_max = std::max(new_z_extremums.first, new_z_extremums.second);
	
	if( (z_min == z_max) || (!new_base_contour->IsValid()) ){
		errors_log->AddError(ERROR_WORK_FIELD_NOT_VALID);
		valid = false;
		return;
	}
	
	z_extremums = std::make_pair(z_min, z_max);
	base_contour = new_base_contour;
	valid = true;
}

WorkField::~WorkField(){
	delete base_contour;
}

bool WorkField::IsValid() const {
	return valid;
}

bool WorkField::GenerateFieldContour(const double &z_plane, ErrorsLog *errors_log, Contour **result_contour){
	std::list<Point3D> new_waypoints;
	Point3D point_to_correct;
	ErrorsLog local_errors_log;
	
	if(!IsValid()){
		errors_log->AddError(ERROR_WORK_FIELD_NOT_VALID);
		return false;
	}
	
	if( (z_plane < z_extremums.first) || (z_plane > z_extremums.second) )
		errors_log->AddWarning(WARNING_REQUESTED_PLANE_OUTSIDE_OF_WORK_FIELD_LIMITS);
	
	for(auto &it : base_contour->GetWaypoints(&local_errors_log)){
		point_to_correct = it;
		point_to_correct.z = z_plane;
		new_waypoints.push_back(point_to_correct);
	}
	
	if(local_errors_log.HaveErrors()){
		valid = false;
		errors_log->AddError(ERROR_WORK_FIELD_NOT_VALID);
		return false;
	}
	
	*result_contour = new Contour(new_waypoints, &local_errors_log);

	if(local_errors_log.HaveErrors()){
		delete *result_contour;
		errors_log->AddError(ERROR_WORK_FIELD_NOT_VALID);
		return false;
	}
	
	std::cout<<"field contour generated"<<std::endl;
	(*result_contour)->PrintContour();
	std::cout<<*result_contour<<std::endl;
	return true;
}

ContoursAggregator::ContoursAggregator(const std::vector<std::list<Point3D>> source_contours, ErrorsLog *errors_log){
	Contour *new_contour;
	ErrorsLog local_errors_log;
	bool abort = false;
	double z_plane_new_contour;
	std::map<double, std::set<Contour*>>::iterator it_raw_contours;
	
	for(auto &it : source_contours){
		new_contour = new Contour(it, &local_errors_log);
		
			//errors handling
		if(local_errors_log.HaveErrors()){
			delete new_contour;
			errors_log->CopyErrors(&local_errors_log);
			abort = true;
			if (abort)
				std::cout<<"Aborted during contour creation"<<std::endl;
			break;
		}
		std::cout<<"Contour done!"<<std::endl;
			
			//everything fine with this contour, put it into raw_contours
				//check z_plane of new contour and validity
		if(new_contour->IsValid()){
			z_plane_new_contour = new_contour->GetZPlane();
		}	
		else{
				//if not valid, make error and abort:
			abort = true;
			std::cout<<"Aborted when putting in raw_contours"<<std::endl;
			errors_log->AddError(ERROR_CONTOUR_NOT_VALID);
			break;
		}
				//everything fine, add new contour to map "raw_contours"
		it_raw_contours = raw_contours.find(z_plane_new_contour);
		if(it_raw_contours == raw_contours.end()){
			raw_contours.insert( {z_plane_new_contour, {new_contour} } );
		}
		else{
			it_raw_contours->second.insert(new_contour);
		}
	}
	
		//if abort, delete everything what was created
	if(abort){
		for(auto &it_sets : raw_contours){
			for(auto &it_contours : it_sets.second)
				delete it_contours;
		}
		return;
	}
	
	workfield = NULL;
	
	
	for(auto &it_map : raw_contours){
		std::cout<<"trying to print contour"<<std::endl;
		for(auto &it_set : it_map.second)
			it_set->PrintContour();
	}
}

ContoursAggregator::~ContoursAggregator(){
	for(auto &it_map : raw_contours){
		for(auto &it_set : it_map.second)
			delete it_set;
	}
	
	for(auto &it_map : path_pattern){
		for(auto &it_set : it_map.second)
			delete it_set;
	}
	
	delete workfield;
}

std::list<Point3D> ContoursAggregator::GenerateEquidistantContourFragment(const Point3D &prev_wpt, const Point3D &current_wpt, const Point3D &next_wpt){
	std::list<Point3D> result_list;
	
	//Make something
	
	
	return result_list;
}

bool ContoursAggregator::EquidistantSinglePointContour(const Point3D &single_wpt, const double &spacing, Contour* equidistant_contour){
	std::cout<<"Equidistant contour for single point function is not yet done"<<std::endl;
	return false;
}

bool ContoursAggregator::Equidistant(const Contour* source_contour, const double &spacing, Contour* new_equidistant_contour){
	std::cout<<"Inside equidistant funсtion"<<std::endl;
	
	std::list<Point3D> new_wpts;
	std::list<Point3D> source_wpts;
	std::list<Point3D> equidistant_fragment;
	ErrorsLog local_errors_log;
	
	source_wpts = source_contour->GetWaypoints(&local_errors_log);
	
		//case when single-point contour to be equidistanted
	if(source_wpts.size() == 1)
		return EquidistantSinglePointContour(*source_wpts.begin(), spacing, new_equidistant_contour);
	
	assert(source_wpts.size() > 2);
	
	for(auto current_wpt = source_wpts.begin(); current_wpt != source_wpts.end(); current_wpt++){
			//setup next and previous waypoint
		auto next_wpt = current_wpt;
		auto prev_wpt = current_wpt;
		next_wpt++;
		if(next_wpt == source_wpts.end())
			next_wpt = source_wpts.begin();
		if(current_wpt == source_wpts.begin())
			prev_wpt = source_wpts.end();
		prev_wpt--;
		
			//make equidistant fragment over current waypoint (using also prev and next wpts)
		equidistant_fragment = GenerateEquidistantContourFragment(*prev_wpt, *current_wpt, *next_wpt);
		
			//insert each new fragment in the end of new contour waypoints list
		new_wpts.insert(new_wpts.end(), equidistant_fragment.begin(), equidistant_fragment.end());
	}
	
		//try to make contour from new waypoints
	std::cout<<"Need to make check for self-crossing for contours"<<std::endl;
	new_equidistant_contour = new Contour(new_wpts, &local_errors_log);
		
		//errors handling
	if(local_errors_log.HaveErrors()){
			delete new_equidistant_contour;
			return false;
		}
	return true;
}


bool ContoursAggregator::AssignWorkField(WorkField *new_workfield){
	if(new_workfield->IsValid()){
		workfield = new_workfield;
		return true;
	}
	else{
		workfield = NULL;
		return false;
	}
}

std::vector<Contour*> ContoursAggregator::GetPreparedContours(const double &spacing,  ErrorsLog *errors_log){
	assert(workfield != NULL);
	
	std::cout<<"inside GetPreparedContours function"<<std::endl;

	std::cout<<"MAKE FUNCTION FOR CHECKING THIN AREAS IN RAW CONTOURS VECTORS!!!"<<std::endl;
	
	std::vector<Contour*> result_contours;
	std::map<double, std::vector<Contour*>> all_active_contours;				//contours on outer layers, mapped by z coordinate
	std::vector<Contour*> current_z_active_contours;										// to be check for crossing with new contours
	std::vector<Contour*> new_equidistant_contours;
	Contour *field_contour = NULL;
	ErrorsLog local_errors_log;
	bool abort = false;
	
		//this loop passes all z_planes
	for(auto &it_z_plane : raw_contours){

			//for each new plane need to create field contour
			if(!workfield->GenerateFieldContour(it_z_plane.first, &local_errors_log, &field_contour)){
				errors_log->CopyErrors(&local_errors_log);
				abort = true;
				break;
			}
	
			assert(field_contour != NULL);

			current_z_active_contours.push_back(field_contour);
			//field contour represents workfield boundaries. It was made successfully. 
			//It was added to active contours of this z_plane
			//As of now, this set of active contours consists only of field contour
				
			//this loop passes all raw contours which belong to same z plane
		for(auto &it_raw_contour : it_z_plane.second){
			//Make equidistant contour from every raw contour
			//After equidistanting, contour can transform into more than one contour. 
			//Thats why new contour(s) to be returned in set
			new_equidistant_contours.clear();
			if(!Equidistant(it_raw_contour, spacing, &new_equidistant_contours)){		
				abort = true;
				break;				
			}
			//Every new equidistant contour to be added into active contours. 
			//If it intersects with existing contours, cut overlapping parts
			AddAndSolveIntersections(new_equidistant_contours, spacing, &current_z_active_contours);
		}
		
		std::vector<Contour*> active_contours_iterator;

		//Now all raw contours processed
		//Equidistant active contours one by one until have at least one
		//eache time when active contour is equidistanted, new equidistant is added (if it valid), and old contour is removed
		//so, when last active contour's equidistant is not valid, loop is finished
		//equidistant is valid when it forms at least part of new contour
		//old contour (the one which was equidistanted) to be moved from active contours set to result conoturs set
		while(!current_z_active_contours.empty()){
			new_equidistant_contours.clear();
			active_contours_iterator = current_z_active_contours.begin();
			
			if(!Equidistant(*active_contours_iterator, spacing, &new_equidistant_contours)){
				abort = true;
				break;
			}

			result_contours.push_back(*active_contours_iterator);
			current_z_active_contours.erase(active_contours_iterator);

			AddAndSolveIntersections(new_equidistant_contours, spacing, &current_z_active_contours);
		}
		
		if(abort)
			break;

	}
	
	if(abort){
		std::cout<<"aborted"<<std::endl;
		errors_log->AddError(GENERAL_ERROR);
		for(auto &it: current_z_active_contours)
			delete it;
		for(auto &it: new_equidistant_contours)
			delete it;
		for(auto &it : result_contours)
			delete it;
		delete field_contour;
	}
	std::cout<<"GetPreparedContours done! result size: "<<result_contours.size()<<std::endl;
	return result_contours;
}

WorkField* CreateTestWorkField(){
	ErrorsLog local_errors_log;
	WorkField* new_workfield;
	Contour* new_base_contour;
	
	new_base_contour = new Contour(TEST_WORKFIELD_WAYPOINTS, &local_errors_log);
	
	assert(!local_errors_log.HaveErrors());
	assert(new_base_contour->IsValid());
	
	new_workfield = new WorkField(TEST_WORKFIELD_Z_EXTREMUMS, new_base_contour, &local_errors_log);
	
	
	assert(!local_errors_log.HaveErrors());
	assert(new_workfield->IsValid());

	return new_workfield;
}
