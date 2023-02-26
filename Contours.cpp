#include "Contours.h"

Contour::Contour(const std::list<Point3D> new_waypoints, std::list<Error>* errors_list, ErrorFlag* error_flag){
	if(new_waypoints.size() == 0){
		valid = false;
		error_flag->RiseError();
		errors_list->push_back(ERROR_CONTOUR_NOT_VALID);
		return;
	}
		//check that all waypoints are in same plane
	z_plane = new_waypoints.begin()->z;
	for(auto &it : new_waypoints){
		if(it.z != z_plane){
			valid = false;
			return;
		}
	}
		//everything fine, contour valid
	valid = true;
	waypoints = new_waypoints;
		//remove inner waypoints from stright lines
	RemoveExessivePoints();
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
	
	if( (z_min == z_max) || (new_base_contour->IsValid()) ){
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

bool WorkField::GenerateFieldContour(const double &z_plane, ErrorsLog *errors_log, Contour *result_contour) const {
	std::list<Point3D> new_waypoints;
	Point3D point_to_correct;
	ErrorsLog local_errors_log;
	
	if(!IsValid()){
		errors_log->AddError(ERROR_WORK_FIELD_NOT_VALID);
		return false;
	}
	
	if( (z_plane < z_extremums.first) || (z_plane > z_extremums.second) )
		errors_log->AddWarning(WARNING_REQUESTED_PLANE_OUTSIDE_OF_WORK_FIELD_LIMITS);
	
	for(auto &it : base_contour->GetWaypoints(&local_errors_log){
		point_to_correct = it;
		point_to_correct.z = z_plane;
		new_waypoints.push_back(point_to_correct);
	}
	
	if(local_errors_log->HaveErrors(){
		valid = false;
		errors_log->AddError(ERROR_WORK_FIELD_NOT_VALID);
		return false;
	}
	
	delete result_contour;
	result_contour = new Contour(new_waypoints, &local_errors_log);
	
}

ContoursAggregator::ContoursAggregator(const std::vector<std::list<Point3D>> source_contours, std::list<Error> *errors_list, ErrorFlag *error_flag){
	Contour *new_contour;
	std::list<Error> local_errors;
	ErrorFlag local_error_flag;
	bool abort = false;
	double z_plane_new_contour;
	std::map<double, std::set<Contour*>>::iterator it_raw_contours;
	
	for(auto &it : source_contours){
		new_contour = new Contour(it, &local_errors, &local_error_flag);
		
			//errors handling
		if(local_error_flag.fatal_error_flag || local_error_flag.error_flag){
			delete new_contour;
			for(auto &it_error : local_errors)
				errors_list->push_back(it_error);
			error_flag->fatal_error_flag = local_error_flag.fatal_error_flag;
			error_flag->error_flag = local_error_flag.error_flag;
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
			error_flag->error_flag = true;
			errors_list->push_back(ERROR_CONTOUR_NOT_VALID);
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
	
	for(auto &it_map : outer_contours){
		for(auto &it_set : it_map.second)
			delete it_set;
	}
	
	for(auto &it_map : contours_ready_to_make_path){
		for(auto &it_set : it_map.second)
			delete it_set;
	}
	delete workfield;
}

void ContoursAggregator::AssignWorkField(WorkField *new_workfield, ErrorsLog *errors_log){
	if(new_workfield->IsValid())
		workfield = new_workfield;
	else{
		workfield = NULL;
		errors_log->AddError(ERROR_WORK_FIELD_NOT_VALID);
	}
}

std::vector<Contour*> ContoursAggregator::GetPreparedContours(const double &spacing,  ErrorsLog *errors_log){
	assert(workfield != NULL);
		//this loop passes all z_planes
	for(auto &it_z_plane : raw_contours){
			//first equidistant and merge field conotur
			EquidistantAndMerge(
			//this loop passes all raw contours which belong to same z plane
		for(auto &it_contour_set : it_z_plane.second){
			EquidistantAndMerge(it_contour_set, spacing);
		}
		
	}
	
}