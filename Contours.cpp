#include "Contours.h"

Contour::Contour(const std::list<Point3D> new_waypoints, std::list<Error>* errors_list, ErrorFlag* error_flag){
	if(new_waypoints.size() == 0){
		error_flag->RiseError();
		errors_list->push_back(ERROR_CONTOUR_NOT_VALID);
		return;
	}
	waypoints = new_waypoints;
}

Contour::~Contour(){}

double Contour::GetZPlane(bool *valid) const{
	auto it = waypoints.begin();
	std::cout<<waypoints.size()<<std::endl;
	if(it == waypoints.end()){
		*valid = false;
		return 0;
	}
	double first_z = it->z;
	it++;
	while(it != waypoints.end()){
		if(it->z != first_z){
			*valid = false;
			return 0;
		}
		it++;
	}
	*valid = true;
	return first_z;
}

void Contour::PrintContour() const{
	for(auto &it : waypoints)
		Print(it);
}

ContoursAggregator::ContoursAggregator(const std::vector<std::list<Point3D>> source_contours, std::list<Error> *errors_list, ErrorFlag *error_flag){
	std::cout<<"Inside Contours aggregator constructor"<<std::endl;
	Contour *new_contour;
	std::list<Error> local_errors;
	ErrorFlag local_error_flag;
	bool abort = false;
	bool valid_new_contour = false;
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
		z_plane_new_contour = new_contour->GetZPlane(&valid_new_contour);
				//if not valid, make error and abort:
		if(!valid_new_contour){
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
	
	for(auto &it_map : raw_contours){
		std::cout<<"trying to print contour"<<std::endl;
		for(auto &it_set : it_map.second)
			it_set->PrintContour();
	}
}

ContoursAggregator::~ContoursAggregator(){}

