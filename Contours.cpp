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
	return true;
}

std::set<double> ContoursAggregator::GetAllZPlanesOfRawContours() const{
	std::set<double> result_set;
	for(auto &it : raw_contours)
		result_set.insert(it.first);
	return result_set;
}

std::set<Contour*> ContoursAggregator::	GetRawContoursByZPlane(const double &z_plane) const{
	std::map<double, std::set<Contour*>>::const_iterator map_it;
	std::set<Contour*> result_set;
	map_it = raw_contours.find(z_plane);

	if(map_it == raw_contours.end())
		return result_set;

	result_set = map_it->second;
	return result_set;
}

bool ContoursAggregator::AddContourToPreparedContours(Contour* new_prepared_contour){
		//checks
	if(new_prepared_contour == NULL)
		return false;
	if(!new_prepared_contour->IsValid())
		return false;

	//all ok, add contour
	double z_plane = new_prepared_contour->GetZPlane();
	auto it_prepared_contours = prepared_contours.find(z_plane);

	if(it_prepared_contours == prepared_contours.end())
		prepared_contours.insert( std::make_pair(z_plane, std::set<Contour*> {new_prepared_contour}) );
	else
		it_prepared_contours->second.insert(new_prepared_contour);

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
	
	for(auto &it_map : prepared_contours){
		for(auto &it_set : it_map.second)
			delete it_set;
	}
	
	delete workfield;
}

std::list<Point3D> ContoursAggregator::GenerateEquidistantContourFragment(const Point3D &prev_wpt, const Point3D &current_wpt, const Point3D &next_wpt, const double &spacing){

	std::list<Point3D> result_list;
	double rotation_angle, length, bissectris;
	bool sharp_angle;

	MathVector3D vec_1(prev_wpt, current_wpt);
	MathVector3D vec_2(current_wpt, next_wpt);

		//TODO: test case for waypoints on same line
		//if waypoints are on same line
	if(MathOperations::AngleBetweenVectors(vec_1, vec_2) == PI){
		vec_1.Normalize();
		vec_1.Rotate(Z_AXIS, PI/2);
		vec_1.MultiplyByNumber(spacing);
		Point3D new_leg = current_wpt;
		new_leg.Shift(vec_1);
		result_list.push_back(new_leg);
		return result_list;
	}

	MathOperations::VectorMultiplication(vec_1, vec_2).Print();
	sharp_angle = MathOperations::VectorMultiplication(vec_1, vec_2).GetZ() > 0 ? false : true;
	bissectris = MathOperations::AngleBetweenVectors(vec_1, vec_2) / 2;

		//if right-hand angle is sharp, only one waypoint: intersection of equidistant lines
	if(sharp_angle){
		length = spacing / sin(bissectris);
		rotation_angle = PI - bissectris;
		vec_1.Normalize();
		vec_1.Rotate(Z_AXIS, rotation_angle);
		vec_1.MultiplyByNumber(length);
		Point3D new_leg = current_wpt;
		new_leg.Shift(vec_1);
		result_list.push_back(new_leg);
		return result_list;
	}

		//if right-hand angle is not sharp: two waypoints
	
	Point3D new_leg_1, new_leg_2;

	rotation_angle = (PI - bissectris) / 2;
	length = spacing / cos(PI/2 - rotation_angle);

	vec_1.Normalize();
	vec_1.Rotate(Z_AXIS, rotation_angle);
	vec_1.MultiplyByNumber(length);
	new_leg_1 = current_wpt;
	new_leg_1.Shift(vec_1);

	
	vec_2.Normalize();
	vec_2.Rotate(Z_AXIS, PI-rotation_angle);
	vec_2.MultiplyByNumber(length);
	new_leg_2 = current_wpt;
	new_leg_2.Shift(vec_2);

	result_list.push_back(new_leg_1);
	result_list.push_back(new_leg_2);
	
	return result_list;
}

bool ContoursAggregator::EquidistantSinglePointContour(const Point3D &single_wpt, const double &spacing, std::vector<Contour*> *equidistant_contours){
	//TODO: EquidistantSinglePoint function
	std::cout<<"Equidistant contour for single point function is not yet done"<<std::endl;
	return false;
}

bool ContoursAggregator::Equidistant(const Contour* const &source_contour, const double &spacing, std::vector<Contour*> *new_equidistant_contours){

	std::cout<<"Inside equidistant funсtion"<<std::endl;

	std::list<Point3D> new_wpts;
	std::list<Point3D> source_wpts;
	std::list<Point3D> equidistant_fragment;
	ErrorsLog local_errors_log;
	
	new_equidistant_contours->clear();
	source_wpts = source_contour->GetWaypoints(&local_errors_log);
	
		//case when single-point contour to be equidistanted
	if(source_wpts.size() == 1)
		return EquidistantSinglePointContour(*source_wpts.begin(), spacing, new_equidistant_contours);

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
		equidistant_fragment = GenerateEquidistantContourFragment(*prev_wpt, *current_wpt, *next_wpt, spacing);
		
			//insert each new fragment in the end of new contour waypoints list
		new_wpts.insert(new_wpts.end(), equidistant_fragment.begin(), equidistant_fragment.end());
	}
	
		//try to make contour from new waypoints
		//TODO: make check for self-crossing of contours
	std::cout<<"Need to make check for self-crossing for contours"<<std::endl;
	new_equidistant_contours->push_back(new Contour(new_wpts, &local_errors_log));
	(*new_equidistant_contours->rbegin())->PrintContour();

		//errors handling
	if(local_errors_log.HaveErrors()){
			delete *(new_equidistant_contours->rbegin());
			new_equidistant_contours->pop_back();
			return false;
		}
	return true;
}

bool ContoursAggregator::MergeWithActiveContours(const std::vector<Contour*> &contours_to_be_merged, const double &spacing, std::vector<Contour*> *active_contours){
	//TODO:  make MergeWithActiveContours function
	std::cout<<"Function AddAndSolveIntersections not yet done"<<std::endl;
	return false;
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
	
	system("clear");

	std::vector<Contour*> current_z_active_contours;										// to be check for crossing with new contours
	std::vector<Contour*> new_equidistant_contours;
	std::set<double> all_z_planes;
	Contour *field_contour = NULL;
	ErrorsLog local_errors_log;
	bool abort = false;
	
	all_z_planes = GetAllZPlanesOfRawContours();

		//this loop passes all z_planes
	for(auto &it_z_plane : all_z_planes){

			current_z_active_contours.clear();

			//for each new plane need to create field contour
			if(!workfield->GenerateFieldContour(it_z_plane, &local_errors_log, &field_contour)){
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
		for(auto &it_raw_contour : GetRawContoursByZPlane(it_z_plane)){
			//Make equidistant contour from every raw contour
			//After equidistanting, contour can transform into more than one contour. 
			//Thats why new contour(s) to be returned in set
			if(!Equidistant(it_raw_contour, spacing, &new_equidistant_contours)){
				abort = true;
				break;				
			}
			//Every new equidistant contour to be added into active contours. 
			//If it intersects with existing contours, cut overlapping parts
				MergeWithActiveContours(new_equidistant_contours, spacing, &current_z_active_contours);
		}
		
		std::vector<Contour*>::iterator active_contours_iterator;

		//Now all raw contours processed
		//Equidistant active contours one by one until empty
		//eache time when active contour is equidistanted, new equidistant is added (if it valid), and old contour is removed
		//so, when last active contour's equidistant is not valid, loop is finished
		//equidistant is valid when it forms at least part of new contour
		//old contour (the one which was equidistanted) to be moved from active contours set to result contours set
		while(!current_z_active_contours.empty()){

			active_contours_iterator = current_z_active_contours.begin();
			
			if(!Equidistant(*active_contours_iterator, spacing, &new_equidistant_contours)){
				abort = true;
				break;
			}

				//contour assigned to active contours iterator successfully equidistanted, move it to result contours
			AddContourToPreparedContours(*active_contours_iterator);
				//it also can be removed from active contours
			current_z_active_contours.erase(active_contours_iterator);

				//new equidistant contours to be checked for intersection with active contours. If have intersections,
				//solve this by merging new contours with intersected ones.
				//active contoutrs set to be updated (by adding new active contour(s), if applicable).
				//All listed above is performin inside following function:
			MergeWithActiveContours(new_equidistant_contours, spacing, &current_z_active_contours);
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
		for(auto &it_map : prepared_contours)
			for(auto &it_set : it_map.second)
				delete it_set;
		delete field_contour;
	}
	std::cout<<"GetPreparedContours done!"<<std::endl;
	return {};
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
