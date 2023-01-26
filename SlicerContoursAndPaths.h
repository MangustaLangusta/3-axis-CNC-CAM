#ifndef SLICER_CONTOURS_AND_PATHS_H
#define SLICER_CONTOURS_AND_PATHS_H

#define FIELD_WAYPOINTS { {-20, -20}, {-20, 110}, {110, 110}, {110, -20} }
#define FIELD_HEIGHT 150
#define MIN_X -10000
#define MAX_X 10000
#define FEED_OVERLAP_RATIO 0.8 //how much part of instrument radius to be ovelapped

struct BorderLink{
	Coordinate x;
	Coordinate y;
	Point2D coordinates;
	std::vector<Point2D> additional_points;	//connected ponts with same x-coordinate, meaningless for alghoritm being used
	bool start;
	bool finish;
	bool upper;	
	
	
	friend bool operator< (const BorderLink &link_a, const BorderLink &link_b){
		if (link_a.x < link_b.x) 
			return true;
		if (link_a.x == link_b.x){
			if(link_a.y > link_b.y) 
				return true;
			if(link_a.y == link_b.y)
				return link_a.upper;
		}
		return false;
	}
	
	friend bool operator< (const BorderLink &link, const Coordinate x){
		return (link.x < x);
	}
	
	void PrintBorderLink() {
		std::cout<<"x= "<<x<<" y= "<<y<<" start="<<start<<" finish="<<finish<<" upper="<<upper<<std::endl;
		if(!additional_points.empty())
			std::cout<<"additional_points:"<<std::endl;
		for(auto it = additional_points.begin(); it != additional_points.end(); it++)
			Point2D(*it).print_point();
	}
};

class Border{
	private:
		std::list<BorderLink> links;
		std::set<Coordinate> all_x;
		Point2D min_y_point;
		Point2D max_y_point;
		bool upper;
		
		
		
		void EmplaceBorderLink(std::list<BorderLink>::iterator *it_new, BorderLink new_link){
			all_x.insert(new_link.x);
			Point2D new_link_lowest = new_link.coordinates;
			for(auto it : new_link.additional_points)
				new_link_lowest = it.y < new_link_lowest.y ? it : new_link_lowest;
			if(links.empty()){
				min_y_point = new_link_lowest;
				max_y_point = new_link_lowest;
			}
			else {
				min_y_point = new_link.y < min_y_point.y ? new_link_lowest : min_y_point;
				max_y_point = new_link.y > max_y_point.y ? new_link_lowest : max_y_point;
			}
			if(*it_new == links.begin()){
				new_link.start = true;
				links.begin()->start = false;
			}
			else if(*it_new == links.end()){
				new_link.finish = true;
				links.rbegin()->finish = false;
			}
			*it_new = links.emplace(*it_new, new_link);
		}
		
		void AddLinkFront(BorderLink new_link){
			std::list<BorderLink>::iterator *it_link;
			*it_link = links.begin();
			this->EmplaceBorderLink(it_link, new_link);
			std::cout<<"front emplaced link "<<std::endl;
			new_link.PrintBorderLink();
			std::cout<<"in fact front link: "<<std::endl;
			links.begin()->PrintBorderLink();
			
		}
		
		void AddLinkBack(BorderLink new_link){
			std::cout<<"add link 1"<<std::endl;
			std::list<BorderLink>::iterator *it_link;
			*it_link = links.end();
			std::cout<<"add link 2"<<std::endl;
			this->EmplaceBorderLink(it_link, new_link);
		}
		
		void AddLinkBackHealthy(BorderLink new_link){
			links.push_back(new_link);
		}
		
		void ReverseLinks() { 
			links.reverse(); 
			(*links.begin()).start = true;
			(*links.rbegin()).start = false;
			(*links.begin()).finish = false;
			(*links.rbegin()).finish = true;
		}

	public:
		Border(bool upper_border, std::vector<Point2D> source_points){
			Coordinate x;
			Coordinate y;
			Point2D coordinates;
			std::vector<Point2D> points;
			upper = upper_border;
			bool start = true;
			bool finish = false;
			long size = source_points.size();
			min_y_point = max_y_point = source_points[0];
			for(long i = 0; i < size; i++){
				if(source_points[(i+1)%size].x == source_points[i].x){
					points.push_back(source_points[i]);
					min_y_point = source_points[i].y < min_y_point.y ? source_points[i] : min_y_point;
					max_y_point = source_points[i].y > max_y_point.y ? source_points[i] : max_y_point;
					continue;
				}
				points.push_back(source_points[i]);
				min_y_point = source_points[i].y < min_y_point.y ? source_points[i] : min_y_point;
				max_y_point = source_points[i].y > max_y_point.y ? source_points[i] : max_y_point;
				finish = (i == (size-1));
				coordinates = *points.rbegin();
				points.erase(--points.end());
				links.push_back({coordinates.x, coordinates.y, coordinates, points, start, finish, upper_border});
				all_x.insert(coordinates.x);
				start = false;
			}
			//this->PrintBorder();
		}
		
		Border(bool is_upper){
			upper = is_upper;
			links.clear();
			//std::cout<<"created Border"<<std::endl;
		}
		
		Border(std::list<BorderLink> &new_links){
			if(new_links.empty())
				return;
			min_y_point = max_y_point = new_links.begin()->coordinates;
			upper = new_links.begin()->upper;
			for(auto &it : new_links){
				links.emplace_back(it);
				all_x.insert(it.x);
				min_y_point = it.y < min_y_point.y ? it.coordinates : min_y_point;
				max_y_point = it.y > max_y_point.y ? it.coordinates : max_y_point;
			}				
		}
		
		~Border (){
			//std::cout<<"Border removed"<<std::endl;
		}
		
		friend bool operator< (const Border &border_a, const Border &border_b){
			return border_a.min_y_point.y < border_b.min_y_point.y;
		}
		
		Coordinate GetMinY(){ return min_y_point.y; }
		Coordinate GetMaxY() { return max_y_point.y; }
		Coordinate GetXBegin()  { return *all_x.begin(); }
		Coordinate GetXEnd() { return *all_x.rbegin(); }
		std::set<Coordinate> GetAllX() { return all_x; }
		
		void ExtractAllXCoordinates(std::set<Coordinate> &x_set){ 
			for(auto it : all_x)
				x_set.insert(it); 
		}
		
		void CreateInterpolatedLinksByX(std::set<Coordinate> &x_coords_set){
			std::list<BorderLink>::iterator it_next = links.begin();
			std::set<Coordinate>::iterator it_biggest, it_smallest;
			Point2D new_point;
			for(auto it = links.begin(); it != links.end(); it++){
				it_next = it;
				it_next++;
				if(it_next == links.end())
					break;
				it_biggest = upper ? x_coords_set.lower_bound(it_next->x) : x_coords_set.lower_bound(it->x);
				it_smallest = upper ? x_coords_set.lower_bound(it->x) : x_coords_set.lower_bound(it_next->x);
				for(it_smallest; it_smallest != it_biggest; it_smallest++){
					if((*it_smallest == it->x)||(*it_smallest == it_next->x))
						continue;
					if(it_next->additional_points.empty())
						new_point = LinearInterpolation(it->coordinates, it_next->coordinates, *it_smallest);
					else
						new_point = LinearInterpolation(it->coordinates, *(it_next->additional_points.rbegin()), *it_smallest);
					if(upper){
						it++;
						EmplaceBorderLink(&it, BorderLink {new_point.x, new_point.y, new_point, {}, false, false, it->upper});
						//it->PrintBorderLink();
					}
					else {
						EmplaceBorderLink(&it_next, BorderLink {new_point.x, new_point.y, new_point, {}, false, false, it->upper});
						//it_next->PrintBorderLink();
					}
				}
			}		
			//this->PrintBorder();
		}
		
		void ClearBorder(){
			links.clear();
			all_x.clear();
		}
		
		std::list<BorderLink> CutLinksByXRange(std::pair<Coordinate, Coordinate> x_range){
			std::list<BorderLink> extracted_list;
			for(auto &it : links){
				if((it.x >= x_range.first)&&(it.x <= x_range.second))
					extracted_list.push_back(it);
				if(((it.x > x_range.second) && upper)||((it.x < x_range.first) && !upper))
					break;
			}
			if(extracted_list.size() < 2)
				extracted_list.clear();
			return extracted_list;
		}
		
		
		
		void SplitBorderByXRange(std::pair<Coordinate, Coordinate> x_range, std::list<BorderLink> &extracted_links, std::vector<std::list<BorderLink>> &split_links){
			std::pair<Coordinate, Coordinate> first_split_range = std::make_pair(MIN_X, x_range.first);
			std::pair<Coordinate, Coordinate> second_split_range = std::make_pair(x_range.second, MAX_X);
			
			std::list<BorderLink> first_split = CutLinksByXRange(first_split_range);
			extracted_links = CutLinksByXRange(x_range);
			std::list<BorderLink> second_split = CutLinksByXRange(second_split_range);
			
			split_links.clear();
			if(!first_split.empty())
				split_links.push_back(first_split);
			if(!second_split.empty())
				split_links.push_back(second_split);
			
		}
		
		void GetLinks(std::list<BorderLink> &links_set) {
			for(auto &it : links)
				links_set.push_back(it);
		}
		
		BorderLink GetLastLink() {
			return *(links.rbegin());
		}
		
		BorderLink GetFirstLink() {
			return *(links.begin());
		}
		
		void PrintBorder() {
			std::cout<<"border min_y = "<<min_y_point.y<<" max_y = "<<max_y_point.y<<" size = "<<links.size()<<std::endl;
			for(auto it = links.begin(); it != links.end(); it++)
				it->PrintBorderLink();
		}
};


class Contour{
	private:
		id ID;
		id parent_ID = 0;
		static id next_ID;
		ContourType type;
		std::vector<Point2D> waypoints;
			//borders are used only for sweep contours//
		Border upper_border = Border(true);												//
		Border lower_border = Border(false);												//
			//----------------------------------------//
		Coordinate z;
		bool loop;
		Offset offset = 0;
	public:
		//raw contour
		Contour(std::multimap <PrecisePoint2D, PrecisePoint2D> *lines_association, double z_plane){
			ID = next_ID++;					//update ID 
			z = z_plane;
			type = raw;
			std::vector<PrecisePoint2D> precise_waypoints; 	//preliminary waypoints
			std::multimap <PrecisePoint2D, PrecisePoint2D>::iterator lines_iter = lines_association->begin();
			precise_waypoints.push_back(lines_iter->first);
			while(lines_iter != lines_association->end()){
				precise_waypoints.push_back(lines_iter->second);
				lines_association->erase(lines_iter);
				lines_iter = lines_association->find(*precise_waypoints.rbegin());
			}
			if(precise_waypoints[0] == precise_waypoints[precise_waypoints.size()-1]){
				precise_waypoints.pop_back();
				loop = true;
			}
			/*
				если контур незамкнут:
			*/
			else{
				loop = false;
			}
			for(auto &it : precise_waypoints)
				waypoints.push_back(it);
			/*
			std::cout<<"Created raw contour: "<<std::endl;
			for(auto &it : waypoints)
				it.print_point();
			*/
		}
		//equidistant contour
		Contour(Contour* parent, Offset init_offset, Coordinate &z_plane){
			ID = next_ID++;					//update ID 
			parent_ID = parent->GetID();
			z = parent->GetZ();
			std::vector<Point2D> base_waypoints = parent->GetWaypoints();
			offset = init_offset;
			z_plane = z;
			long wpts_amount = base_waypoints.size();
			long target = 0;
			long prev = wpts_amount - 1;
			long next = 1;
			Line2D base_line_a = {base_waypoints[prev], base_waypoints[target]};
			Line2D base_line_b = {base_waypoints[target], base_waypoints[next]};
			Line2D equidist_line_a = DrawEquidistantLine(base_line_a, offset);
			Line2D equidist_line_b = DrawEquidistantLine(base_line_b, offset);
			Line2D bissectris, bissectris_normal;
			Point2D equidist_lines_intercept, bissectris_intercept, bissectris_normal_b, wpt_a, wpt_b;
			Coordinate ratio;
			AngleType angle_type;
			do {
				//0. Проверяем угол, который обводим (острый, тупой или развернутый). 
				angle_type = CheckAngleType(equidist_line_a, equidist_line_b);
				if((angle_type != developed) && Lines2DIntercept(equidist_line_a, equidist_line_b, equidist_lines_intercept)){
					if (angle_type == sharp)
						waypoints.push_back(equidist_lines_intercept);
					else {
						bissectris = {base_waypoints[target], equidist_lines_intercept};
						ratio = offset / bissectris.GetLength();
						bissectris_intercept = {base_waypoints[target].x + bissectris.GetDx() * ratio, 
																		base_waypoints[target].y + bissectris.GetDy() * ratio};
						bissectris.GetNormal(bissectris_normal_b);
						bissectris_normal_b.x = bissectris_normal_b.x + bissectris_intercept.x;
						bissectris_normal_b.y = bissectris_normal_b.y + bissectris_intercept.y;
						bissectris_normal = {bissectris_intercept, bissectris_normal_b};
						if(Lines2DIntercept(equidist_line_a, bissectris_normal, wpt_a))
							waypoints.push_back(wpt_a);
						if(Lines2DIntercept(bissectris_normal, equidist_line_b, wpt_b))
							waypoints.push_back(wpt_b);
					}
				}							
				prev = target;
				target = next;
				next = (next + 1) % wpts_amount;
				base_line_b = {base_waypoints[target], base_waypoints[next]};
				equidist_line_a = equidist_line_b;
				equidist_line_b = DrawEquidistantLine(base_line_b, offset);
			}while(target != 0);			
		}
		//field contour
		Contour(ContourType new_type, std::vector<Point2D> wtps, Coordinate height){
			if (new_type == field){
				ID = 0;
				type = field;
				waypoints = FIELD_WAYPOINTS;
				z = FIELD_HEIGHT;
			}
			else
				std::cout<<"WRONG CONSTRUCTOR OF CONTOUR CLASS (FIELD)"<<std::endl;
		}
		//sweep contour
		Contour(ContourType new_type, Border &upper_border, Border &lower_border, const Coordinate &z_plane){
			ID = next_ID++;
			type = sweep;
			z = z_plane;
			this->upper_border = upper_border;
			this->lower_border = lower_border;
			std::list<BorderLink> upper, lower;
			upper_border.GetLinks(upper);
			lower_border.GetLinks(lower);
			for(auto it : upper){
				for(auto it_additional : it.additional_points)
					waypoints.emplace_back(it_additional);
				waypoints.emplace_back(it.coordinates);
			}
			for(auto it : lower){
				for(auto it_additional : it.additional_points)
					waypoints.emplace_back(it_additional);
				if(*waypoints.rbegin() == it.coordinates)
					continue;
				waypoints.emplace_back(it.coordinates);
			}
			/*
			
			checking waypoints overlapping (due to same x coordinate)		******************************
			*/
			std::vector<Point2D>::iterator prev_wpt;
			std::vector<Point2D>::iterator current_wpt = prev_wpt = waypoints.end();
			prev_wpt--;
			prev_wpt--;
			current_wpt--;
			std::vector<Point2D>::iterator next_wpt = waypoints.begin();
			for(next_wpt; next_wpt != waypoints.end(); next_wpt++){
				if((prev_wpt->x == current_wpt->x)&&(current_wpt->x == next_wpt->x)){
					double delta_big = fabs(prev_wpt->y.value - next_wpt->y.value);
					double delta_1 = fabs(prev_wpt->y.value - current_wpt->y.value);
					double delta_2 = fabs(current_wpt->y.value - next_wpt->y.value);
					if((delta_big < delta_1) || (delta_big < delta_2)){
						waypoints.erase(current_wpt);
						if(current_wpt == waypoints.end())
							current_wpt = waypoints.begin();
						continue;
					}
				}
				prev_wpt = current_wpt;
				current_wpt = next_wpt;
			}
			/*
			****************************************************************************************
			*/
			if(*waypoints.begin() == *waypoints.rbegin())
				waypoints.pop_back();
		}
		/*
		//force contour -- for testing
		Contour(std::vector<Point2D> wpts){
			ID = next_ID++;
			waypoints = wpts;
			z = test::test_z;
		}
		*/
		id GetID() { return ID; }
		ContourType GetType() { return type; }
		static id GetNextID() { return next_ID; }
		Coordinate GetZ() { return z; }
		bool IsLoop() { return loop; }
		std::vector<Point2D> GetWaypoints() { return waypoints; }
		Border GetUpperBorder() { return upper_border; }
		Border GetLowerBorder() { return lower_border; }
		/*void GetWaypoints(std::set<Point2D> &request_set){
			for (auto it = waypoints.begin(); it != waypoints.end(); it++)
				if(!request_set.insert(*it).second)
					std::cout<<"check GetWaypoints function for contours. ID = "<<GetID()<<std::endl;
		}*/
		void PrintContour(){
			std::cout<<"Contour z = "<<z<<" ID = "<<ID<<std::endl;
			for(auto it = waypoints.begin(); it != waypoints.end(); it++)
				it->print_point();	
		}
};

id Contour::next_ID = 1;


class PathElement{
	private:
		DekartCoords start;
		DekartCoords finish;
		Speed speed;				//float
		PathType path_type;	//work or travel
	public:
		
};

class Path{
	private:
		Instrument used_instrument;
		std::list<PathElement> elements;
		
	public:
		Path(Border &upper_border, Border &lower_border, Instrument &new_instrument){
			//waypoints should be so arranged, that: 
			//-first waypoint is always most left (min x) point of contour
			used_instrument = new_instrument;
			Coordinate x_step;
			x_step = used_instrument.GetRadius() * FEED_OVERLAP_RATIO;
			//LinearInterpolation(Point2D a, Point2D b, float x)
			//ReverseLine()
			//void CreateInterpolatedLinksByX(std::set<float> &x_coords_set)
			std::set<Coordinate> all_x_set = upper_border.GetAllX();
			std::set<Coordinate> new_x_set = all_x_set;
			if(all_x_set.empty()){
				std::cout<<"x_set of upper border is empty!"<<std::endl;
				return;
			}
			Coordinate dx;
			Coordinate prev_x = *(all_x_set.begin());
			long steps;
			/*
			std::cout<<"all x:"<<std::endl;
			for (auto it : new_x_set)
				std::cout<<it<<" ";
			std::cout<<std::endl;
			*/
			for(auto it : all_x_set){
				dx = it - prev_x;
				//std::cout<<"x = "<<it<<" prev = "<<prev_x<<" dx = "<<dx<<" steps = ";
				if(dx == 0)
					continue;
				steps = ceil(dx.value / x_step.value);
				dx = dx.value / steps;
				//std::cout<<steps<<" new_dx = "<<dx<<std::endl;
				for(auto i_steps = 1; i_steps < (long)steps; i_steps++){
					new_x_set.insert(Coordinate(prev_x.value + dx.value * i_steps));
					//std::cout<<"added "<<prev_x+dx*i_steps<<std::endl;
				}
				prev_x = it;
			}
			//now we have ready new set of x, need to insert new border links according to this new x set
			upper_border.CreateInterpolatedLinksByX(new_x_set);
			lower_border.CreateInterpolatedLinksByX(new_x_set);
			//upper_border.PrintBorder();
			//lower_border.PrintBorder();
			std::list<BorderLink> upper_links, lower_links;
			upper_border.GetLinks(upper_links);
			lower_border.GetLinks(lower_links);
			
				
		}
		
		bool IsEmpty() { return !elements.empty(); }
};

class PathAggregator{
	private:
		std::list<Path> paths;
		bool is_sewed;
	public:
		bool IsSewed(){ return is_sewed; }
		void AddPath(Path new_path){
			
		}
		void ArrangeAndSew(){
			
		}
		
};

class ContoursAndPaths{
	private:
		//Contour work_field = Contour(field, FIELD_WAYPOINTS, FIELD_HEIGHT);	//one field for all objects
		std::map<id, Contour> all_contours;															//all contours are stored here
		std::set<id> raw_contours;																		//all IDs of raw contours 
		std::set<id> equidistant_contours;														//all IDs of equidistant contours
		std::set<id> sweep_contours;																//all IDs of sweep contours
		std::map<id, std::set<id>> contours_tree;										//jerarhy between contours
		std::map<Coordinate, std::set<id>> contours_by_z;									//IDs sorted by z-coordinate
		PathAggregator all_paths;                                   //all path stored here
		
		std::set<id> test_contours;
		
		Coordinate z_step_mm;
		Coordinate z_offset_mm;
////////////////////////////////////////////////////////////////////////////////////////////
		Contour* GetContourByID(id request){
			auto it = all_contours.find(request);
			if (it != all_contours.end())
				return &(it->second);
			else 
				return NULL;
		}
		bool GetIDsByZ(Coordinate z, std::set<id> &request_set){
			auto it = contours_by_z.find(z);
			if (it != contours_by_z.end()){
				request_set = it->second;
				return true;
			}
			return false;
		}
		bool GetBordersByID(id request, Border &upper, Border &lower){
			auto it = all_contours.find(request);
			if(it != all_contours.end()){
				upper = it->second.GetUpperBorder();
				lower = it->second.GetLowerBorder();
				return true;
			}
			return false;
			
		}
		void PrintContour(id request){
			if (GetContourByID(request))
				GetContourByID(request)->PrintContour();
			else 
				std::cout<<"Attempt to print contour with ID "<<request<<" failed: Contour does not exist"<<std::endl;
		}
		void AddToContoursByZ(Coordinate z, id ID){
			std::map<Coordinate, std::set<id>>::iterator it_z = contours_by_z.find(z);
			if (it_z == contours_by_z.end()){
				it_z = contours_by_z.insert( std::make_pair(z, std::set<id>{0, ID}) ).first;
			}
			else
				it_z->second.insert(ID);
		}
		bool GetWaypointsByID(id request, std::vector<Point2D> &wpts){
			std::map<id, Contour>::iterator contours_iter;
			contours_iter = all_contours.find(request);
			if(contours_iter != all_contours.end()){
				wpts = contours_iter->second.GetWaypoints();
				return true;
			}
			return false;
		}
		bool ExtractRawContoursFromLinesSet(std::vector<PreciseLine2D> *lines_set, double z){
			std::multimap <PrecisePoint2D, PrecisePoint2D> lines_association;
			/*
			std::cout<<"lines set to print:"<<std::endl;
			for(auto &it : *lines_set)
				it.PrintLine();
			*/
			for(auto it = lines_set->begin(); it != lines_set->end(); it++){
				lines_association.insert(std::pair<PrecisePoint2D, PrecisePoint2D> (it->a, it->b) );
			}
			//std::cout<<"Lines association done! Size: "<<lines_association.size()<<std::endl;
			if(lines_association.empty())
				return false;
			id tmp_ID;
			while(!lines_association.empty()){
				tmp_ID = Contour::GetNextID();
				all_contours.emplace( std::make_pair(tmp_ID, Contour(&lines_association, z)) );
				raw_contours.insert(tmp_ID);
				AddToContoursByZ(z, tmp_ID);
				//PrintContour(tmp_ID);
			}
			return true;
		}
////////////////////////////////////////////////////////////////////////////////////////////
	public:
		ContoursAndPaths(){
			all_contours.emplace( std::make_pair((id)0, Contour(field, FIELD_WAYPOINTS, FIELD_HEIGHT)) );
			equidistant_contours.insert((id)0);
		}
		void SetZStep(Coordinate new_z_step_mm) { z_step_mm = new_z_step_mm; }
		void SetZOffset(Coordinate new_z_offset_mm) { z_offset_mm = new_z_offset_mm; }
		
		std::set<id> GetAllEquidistantContoursIDs() { return equidistant_contours; }
		
		std::set<id> GetTestContoursIDs() { return test_contours; }
		
		std::set<id> GetAllRawContoursIDs() { return raw_contours; }
		
		void PrintAllContours(){
			for(auto &it : all_contours)
				it.second.PrintContour();
		}
		
///////////////////////////////////////////////////////////////////////////////////////////
		void SplitContourIntoBorders(id requested_ID, std::vector<std::vector<Point2D>> &upper_bounds, std::vector<std::vector<Point2D>> &lower_bounds){
			std::vector<Point2D> cntr, new_bound_upper, new_bound_lower;
			std::vector<long> rising_nodes, falling_nodes, *start_nodes, *end_nodes;
			if (!GetWaypointsByID(requested_ID, cntr))
				return;
			long size = cntr.size();
			bool increasing = (cntr[0].x < cntr[1 % size].x) ? true : false;
			bool decreasing = (cntr[0].x > cntr[1 & size].x) ? true : false;
			for (long i = 1; i <= size; i++){
				if(cntr[i % size].x == cntr[(i+1) % size].x)
					continue;
				if(cntr[i % size].x > cntr[(i+1) % size].x){
					if(!decreasing)
						falling_nodes.push_back(i % size);
					increasing = false;
					decreasing = true;
					continue;
				}
				if(cntr[i % size].x < cntr[(i+1) % size].x){
					if(!increasing)
						rising_nodes.push_back(i % size);
					increasing = true;
					decreasing = false;
				}
			}
			if(falling_nodes.size() != rising_nodes.size()){
				std::cout<<"Sizes of rising and falling vectors are not same!"<<std::endl;
				return;
			}
			if(rising_nodes.empty()){
				std::cout<<"No nodes found!"<<std::endl;
				return;
			} 
			bool rising; 
			long rising_index = 0, falling_index = 0;
			long start_index = rising_nodes[0] < falling_nodes[0] ? rising_nodes[0] : falling_nodes[0];
			for(long i = start_index; i <= (start_index+size); i++){
				if((i % size) == rising_nodes[rising_index]){
					rising = true;
					if(!new_bound_lower.empty()){
						new_bound_lower.push_back(cntr[i % size]);
						lower_bounds.push_back(new_bound_lower);
						new_bound_lower.clear();
					}
					rising_index = (rising_index + 1) % rising_nodes.size();
				}
				if((i % size) == falling_nodes[falling_index]){
					rising = false;
					if(!new_bound_upper.empty()){
						new_bound_upper.push_back(cntr[i % size]);
						upper_bounds.push_back(new_bound_upper);
						new_bound_upper.clear();
					}
					falling_index = (falling_index + 1) % falling_nodes.size();
				}
				if(rising)
					new_bound_upper.push_back(cntr[i % size]);
				else
					new_bound_lower.push_back(cntr[i % size]);
			}
		}
///////////////////////////////////////////////////////////////////////////////////////////
		bool MakeRawContours(FacetsSet *facets_set){
			std::vector<PreciseLine2D> lines_set;
			double z;
			long int layers = facets_set->GetMaxZ() / z_step_mm.value + z_offset_mm.value / z_step_mm.value;	//подсчитываем количество слоев
			if((z_offset_mm / z_step_mm) < 1)
				layers++;
			/*
				Далее для каждого слоя находим пересечение фигуры с соответствующей плоскостью z и создаем замкнутый контур так, чтобы по направлению обхода контура
				пустое пространство находилось справа, а заполненное - слева
			*/
			for(long int i = layers; i >= 0; i--) {
				z = i * z_step_mm.value;
				/*
					если есть хоть какое-нибудь пересечение набора фасетов и плоскости z, то входим в функцию
					все пересечения (если были) сохранены в векторе lines_set. Линии не связаны. Нужно их связать.
				*/
				if( facets_set->InterceptionLinesOnZPlane(z, &lines_set) ){										
					ExtractRawContoursFromLinesSet(&lines_set, z);														//now we convet set of lines into contours 
				}
			}
			std::cout<<"Raw contours extracted"<<std::endl;
			return true;
		}
///////////////////////////////////////////////////////////////////////////////////////////
		bool MakeEquidistantContours(Offset offset, std::set<id> request_ids){
			std::cout<<"Making euidistant contours.."<<std::endl;
			id tmp_ID;
			Coordinate tmp_z;
			Contour *parent_contour;
			for (auto it = request_ids.begin(); it != request_ids.end(); it++){
				parent_contour = GetContourByID(*it);
				tmp_ID = Contour::GetNextID();
				all_contours.emplace( std::make_pair(tmp_ID, Contour(parent_contour, offset, tmp_z)) );
				equidistant_contours.insert(tmp_ID);
				AddToContoursByZ(tmp_z, tmp_ID);
				//PrintContour(tmp_ID);
			}
			std::cout<<"Equidistant contours done!"<<std::endl;
			return true;
		}
///////////////////////////////////////////////////////////////////////////////////////////
		bool MakeSweepContours(Coordinate z){
			auto it_z = contours_by_z.find(z);
			if (it_z == contours_by_z.end())
				return false;
			std::cout<<"Start making sweep contours.."<<std::endl;
			std::vector<Point2D> wpts;
			std::set<id> ID_set_by_z = it_z->second;
			std::set<id> ID_equidistant_contorus = GetAllEquidistantContoursIDs();
			std::vector<id> ID_set_to_make;
			
			std::set_intersection(ID_set_by_z.begin(), ID_set_by_z.end(), ID_equidistant_contorus.begin(), ID_equidistant_contorus.end(),
                          std::back_inserter(ID_set_to_make));
			
			for(auto &it : ID_set_to_make)
				std::cout<<it<<" ";
			std::cout<<std::endl;
			
			//Now we have set of all points, 
			//this set is sorted by x (smallest to largest) and (if x1 == x2) by y (smallest to largest)
			
			std::vector<std::vector<Point2D>> upper_bounds, lower_bounds;
			std::vector<Border> lower_borders, upper_borders;
			std::set<Coordinate> all_x;
			for(auto it : ID_set_to_make){
				/*if(it == 0)
					continue;
				*/
				std::cout<<"ID = "<<it<<std::endl;
				PrintContour(it);
				SplitContourIntoBorders(it, upper_bounds, lower_bounds);
				for(auto it = upper_bounds.begin(); it != upper_bounds.end(); it++){
					upper_borders.push_back(Border(true, *it));
				}
				for(auto it = lower_bounds.begin(); it != lower_bounds.end(); it++)
					lower_borders.push_back(Border(false, *it));			
				upper_bounds.clear();
				lower_bounds.clear();
			}
			for(auto &it : lower_borders)
				it.ExtractAllXCoordinates(all_x);
			for(auto &it : upper_borders)
				it.ExtractAllXCoordinates(all_x);
		
			for(auto &it : lower_borders){
				it.CreateInterpolatedLinksByX(all_x);
				it.PrintBorder();
			}
			for(auto &it : upper_borders){
				it.CreateInterpolatedLinksByX(all_x);
				it.PrintBorder();
			}
			/*
			variables used in following loop
			*/
			std::set<Coordinate> x_LU_set, x_set_to_exclude, x_set_begins, x_set_ends, x_set_tmp, x_LL_set; 
			std::vector<Border>::iterator it_LL_border, it_LU_border;
			std::list<BorderLink> new_upper_links, new_lower_links;
			std::vector<std::list<BorderLink>> split_links_upper, split_links_lower;
			std::pair<Coordinate, Coordinate> x_range;
			std::set<Coordinate> x_set_LL;
			std::set<Coordinate>::iterator x_start_LL, x_end_LL;
			bool valid_first;
			bool valid_second;
			Border extracted_border_upper(true);
			Border extracted_border_lower(false);
			id tmp_ID;
			/* */
			
			
			
			while(!upper_borders.empty() || !lower_borders.empty()) {
				//system("cls");
				x_set_begins.clear();
				x_set_ends.clear();
				x_set_to_exclude.clear();
				valid_first = false;
				valid_second = false;
				//std::cout<<"before clearing of new borders"<<std::endl;
				extracted_border_upper.ClearBorder();
				extracted_border_lower.ClearBorder();
				
				//std::cout<<"before sorting of borders"<<std::endl;
				std::sort(upper_borders.begin(), upper_borders.end());
				std::sort(lower_borders.begin(), lower_borders.end());
			
				it_LU_border = upper_borders.begin();
				//std::cout<<"upper border to work on:"<<std::endl;
				//it_LU_border->PrintBorder();
				
				//to find lowest upper border and upper borders that min_y < lowest upper border max_y
				
				x_LU_set = it_LU_border->GetAllX();	//ALL X COORDS OF LU BORDER			
				
				
			
				//this can be improved
				for(auto it_other_upper = it_LU_border + 1; it_other_upper != upper_borders.end(); it_other_upper++){
					if(it_other_upper->GetMinY() > it_LU_border->GetMaxY())
						break;
					x_set_begins.insert(it_other_upper->GetXBegin());
					x_set_ends.insert(it_other_upper->GetXEnd());	
					x_set_tmp = it_other_upper->GetAllX();
					for(auto it = ++x_set_tmp.begin(); it != --x_set_tmp.end(); it++)
						x_set_to_exclude.insert(*it);
				}
				for(auto &it : x_set_begins) {
					if(x_set_ends.find(it) != x_set_ends.end())
						x_set_to_exclude.insert(it);
				}
				//now x_set_to_exclude contains all intersected x of possible overcasting upper borders			
				
				//std::cout<<"found x_set_to_exclude"<<std::endl;
				for(auto it : x_LU_set){
					if(x_set_to_exclude.find(it) == x_set_to_exclude.end()){
						if(valid_first){
							x_range.second = it;
							valid_second = true;
						}
						else {
							valid_first = true;
							x_range.first = it;
						}
					}
					else {
						if(valid_second){
							//here we try to check if we have appropriate lower border
							bool good_solution = false;
							//std::cout<<"initial range try: "<<x_range.first<<" "<<x_range.second<<std::endl;
							for(it_LL_border = lower_borders.begin(); it_LL_border != lower_borders.end(); it_LL_border++){
								x_set_LL = it_LL_border->GetAllX();
								x_start_LL = x_set_LL.lower_bound(x_range.first);
								if(x_start_LL == x_set_LL.end())
									continue;
								x_end_LL = x_set_LL.upper_bound(x_range.second);
								if(x_end_LL == x_set_LL.end())
									continue;
								x_end_LL--;
								if(x_end_LL == x_start_LL)
									continue;
								good_solution = true;
								x_range.first = *x_start_LL;
								x_range.second = *x_end_LL;
								//std::cout<<"final solution: "<<x_range.first<<" "<<x_range.second<<std::endl;
								break;
							}
							if(good_solution)
								break;
							valid_first = false;
							valid_second = false;
						}
						else{
							valid_first = false;
						}
					}
				}
				
//EVERYTHING LOOKS OK TILL THIS PLACE
				/*
					new_upper_links, new_lower_links;
					std::vector<std::list<BorderLink>> split_links_upper, split_links_lower;
			*/
				
				
				if(valid_first && valid_second){
					//std::cout<<"found x_range: "<<x_range.first<<" "<<x_range.second<<std::endl;
					it_LU_border->SplitBorderByXRange(x_range, new_upper_links, split_links_upper);
					it_LL_border->SplitBorderByXRange(x_range, new_lower_links, split_links_lower);
				}
				else {
					std::cout<<"Did not found good solution..."<<std::endl;
				}
				
				extracted_border_upper = Border(new_upper_links);
				extracted_border_lower = Border(new_lower_links);
				/*
				std::cout<<"extracted upper: "<<std::endl;
				extracted_border_upper.PrintBorder();
				std::cout<<"extracted lower: "<<std::endl;
				extracted_border_lower.PrintBorder();
				*/
				//now we need to modify upper and lower borders from main vectors (to exchange with new borders)
				//also need to sort borders - this was done in the beginning of loop

				upper_borders.erase(it_LU_border);
				lower_borders.erase(it_LL_border);
				for(auto &it : split_links_upper)
					upper_borders.emplace_back(Border(it));
				for(auto &it : split_links_lower)
					lower_borders.emplace_back(Border(it));
								
				//now we can create sweep contour from our borders
				tmp_ID = Contour::GetNextID();
				all_contours.emplace( std::make_pair(tmp_ID, Contour(sweep, extracted_border_upper, extracted_border_lower, z)) );
				sweep_contours.insert(tmp_ID);
				AddToContoursByZ(z, tmp_ID);
				/*
				PrintContour(tmp_ID);
				std::cout<<"other upper borders: "<<std::endl;
				for(auto it : upper_borders)
					it.PrintBorder();
				std::cout<<"other lower borders: "<<std::endl;
				for(auto it : lower_borders)
					it.PrintBorder();*/
				//std::cout<<std::endl;
				//system("pause");
			}
			
			
			
			return true;
		}
		
	void MakePathFromSweepContour(id sweep_contour_id, Instrument &used_instrument){
		Border upper_border = Border(true);
		Border lower_border = Border(false);
		//bool GetBordersByID(id request, Border &upper, Border &lower)
		if(!GetBordersByID(sweep_contour_id, upper_border, lower_border)){
			std::cout<<"Warning: sweep contour does not exist"<<std::endl;
			return;
		}
		std::cout<<"making path..."<<std::endl;
		//Path(Border &upper_border, Border &lower_border, Instrument &new_instrument)
		Path new_path(upper_border, lower_border, used_instrument);		
		all_paths.AddPath(new_path);
	}
	
	void MakePathsFromAllSweepContours(Instrument &used_instrument){
		Contour *pt_contour;
		
		for(auto &it : sweep_contours){
			pt_contour = GetContourByID(it);
			pt_contour->PrintContour();
			MakePathFromSweepContour(it, used_instrument);
		}
		
	}
	/*
	void MakeTestContours(){
		id tmp_ID = Contour::GetNextID();
		all_contours.emplace( std::make_pair(tmp_ID, Contour(test::test_contour)) );
		test_contours.insert(tmp_ID);
		AddToContoursByZ(test::test_z, tmp_ID);
		tmp_ID = Contour::GetNextID();
		all_contours.emplace( std::make_pair(tmp_ID, Contour(test::test_field)) );
		test_contours.insert(tmp_ID);
		AddToContoursByZ(test::test_z, tmp_ID);
	}
	*/
	
};





#endif
