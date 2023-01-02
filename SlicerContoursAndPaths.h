#ifndef SLICER_CONTOURS_AND_PATHS_H
#define SLICER_CONTOURS_AND_PATHS_H

#define FIELD_WAYPOINTS { {-10, -10}, {-10, 110}, {110, 110}, {110, -10} }
#define FIELD_HEIGHT 150

struct BorderLink{
	const float x;
	const float y;
	const Point2D coordinates;
	const std::vector<Point2D> additional_points;	//connected ponts with same x-coordinate, meaningless for alghoritm being used
	const bool start;
	const bool finish;
	const bool upper;	
	
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
	
	void PrintBorderLink(){
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
		std::set<float> all_x;
		
	public:
		Border(bool upper_border, std::vector<Point2D> source_points){
			float x;
			float y;
			Point2D coordinates;
			std::vector<Point2D> points;
			bool upper = upper_border;
			bool start = true;
			bool finish = false;
			long size = source_points.size();
			for(long i = 0; i < size; i++){
				if(source_points[(i+1)%size].x == source_points[i].x){
					points.push_back(source_points[i]);
					continue;
				}
				points.push_back(source_points[i]);
				finish = (i == (size-1));
				coordinates = *points.rbegin();
				points.erase(--points.end());
				links.push_back({coordinates.x, coordinates.y, coordinates, points, start, finish, upper_border});
				all_x.insert(coordinates.x);
				start = false;
			}
		}
		
		void ExtractAllXCoordinates(std::set<float> &all_x){
		for(auto &it: links)
				all_x.insert(it.x);
		}
		
		void CreateInterpolatedLinksByX(std::set<float> &x_coords_set){
			std::set<float>::iterator it_x_set = x_coords_set.begin();
			std::list<BorderLink>::iterator it_link = links.begin();
			std::list<BorderLink>::iterator it_link_next = it_link;
			Point2D new_point;
			for(long i = 0; i < links.size(); i++){
				it_link_next++;
				while((*it_x_set < it_link->x)||(it_x_set != x_coords_set.end())){
					it_x_set++;
				}
				if(it_x_set == x_coords_set.end())
					break;
				while((*it_x_set < it_link_next->x)||(it_x_set != x_coords_set.end())){
					new_point = LinearInterpolation(it_link->coordinates, it_link_next->coordinates, *it_x_set);
					it_link = links.emplace(it_link, BorderLink {new_point.x, new_point.y, new_point, {}, false, false, it_link->upper});
					it_x_set++;
				}
				it_link++;
			}
		}
		
		void GetLinks(std::set<BorderLink> &links_set){
			for(auto &it : links)
				links_set.emplace(it);
		}
		
		void PrintBorder(){
			std::cout<<"border"<<std::endl;
			for(auto &it : links)
				it.PrintBorderLink();
		}
};


class Contour{
	private:
		id ID;
		id parent_ID = 0;
		static id next_ID;
		ContourType type;
		std::vector<Point2D> waypoints;
		float z;
		bool loop;
		Offset offset = 0;
	public:
		//raw contour
		Contour(std::multimap <Point2D, Point2D> *lines_association, float z_plane){
			ID = next_ID++;					//update ID 
			z = z_plane;
			type = raw;
			std::multimap <Point2D, Point2D>::iterator lines_iter = lines_association->begin();
			waypoints.push_back(lines_iter->first);
			while(lines_iter != lines_association->end()){
				waypoints.push_back(lines_iter->second);
				lines_association->erase(lines_iter);
				lines_iter = lines_association->find(*waypoints.rbegin());
			}
			if(waypoints[0] == waypoints[waypoints.size()-1]){
				waypoints.pop_back();
				loop = true;
			}
			/*
				если контур незамкнут:
			*/
			else{
				loop = false;
			}
		}
		//equidistant contour
		Contour(Contour* parent, Offset init_offset, float &z_plane){
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
			float ratio;
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
						bissectris_normal_b.x += bissectris_intercept.x;
						bissectris_normal_b.y += bissectris_intercept.y;
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
		Contour(ContourType new_type, std::vector<Point2D> wtps, float height){
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
		Contour(ContourType new_type){
			if (new_type == sweep){
				ID = next_ID++;
				type = sweep;
				
			}
			else
				std::cout<<"WRONG CONSTRUCTOR OF CONTOUR CLASS (SWEEP)"<<std::endl;
		}
		//force contour -- for testing
		Contour(std::vector<Point2D> wpts){
			ID = next_ID++;
			waypoints = wpts;
			z = test::test_z;
		}
		id GetID() { return ID; }
		ContourType GetType() { return type; }
		static id GetNextID() { return next_ID; }
		float GetZ() { return z; }
		bool IsLoop() { return loop; }
		std::vector<Point2D> GetWaypoints() { return waypoints; }
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


class Path {
	private:
		Instrument used_instrument;
		float speed;
		
	public:
		
};



class ContoursAndPaths {
	private:
		//Contour work_field = Contour(field, FIELD_WAYPOINTS, FIELD_HEIGHT);	//one field for all objects
		std::map<id, Contour> all_contours;															//all contours are stored here
		std::set<id> raw_contours;																		//all IDs of raw contours 
		std::set<id> equidistant_contours;														//all IDs of equidistant contours
		std::set<id> sweep_contours;																//all IDs of sweep contours
		std::map<id, std::set<id>> contours_tree;										//jerarhy between contours
		std::map<float, std::set<id>> contours_by_z;									//IDs sorted by z-coordinate
		
		std::set<id> test_contours;
		
		float z_step_mm;
		float z_offset_mm;
		float precision;
////////////////////////////////////////////////////////////////////////////////////////////
		Contour* GetContourByID(id request){
			auto it = all_contours.find(request);
			if (it != all_contours.end())
				return &(it->second);
			else 
				return NULL;
		}
		bool GetIDsByZ(float z, std::set<id> &request_set){
			auto it = contours_by_z.find(z);
			if (it != contours_by_z.end()){
				request_set = it->second;
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
		void AddToContoursByZ(float z, id ID){
			std::map<float, std::set<id>>::iterator it_z = contours_by_z.find(z);
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
		bool ExtractRawContoursFromLinesSet(std::vector<Line2D> *lines_set, float z){
			std::multimap <Point2D, Point2D> lines_association;
			for(auto it = lines_set->begin(); it != lines_set->end(); it++){
				lines_association.insert(std::pair<Point2D, Point2D> (it->a, it->b) );
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
		void SetZStep(float new_z_step_mm) { z_step_mm = new_z_step_mm; }
		void SetZOffset(float new_z_offset_mm) { z_offset_mm = new_z_offset_mm; }
		void SetPrecision(float new_precision_mm) { precision = new_precision_mm; }
		
		std::set<id> GetTestContoursIDs() { return test_contours; }
		
		std::set<id> GetAllRawContoursIDs() { return raw_contours; }
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
			std::vector<Line2D> lines_set;
			float z;
			long int layers = facets_set->GetMaxZ() / z_step_mm + z_offset_mm / z_step_mm;	//подсчитываем количество слоев
			if((z_offset_mm / z_step_mm) < 1)
				layers++;
			/*
				Далее для каждого слоя находим пересечение фигуры с соответствующей плоскостью z и создаем замкнутый контур так, чтобы по направлению обхода контура
				пустое пространство находилось справа, а заполненное - слева
			*/
			for(long int i = layers; i >= 0; i--) {
				z = i * z_step_mm;
				/*
					если есть хоть какое-нибудь пересечение набора фасетов и плоскости z, то входим в функцию
					все пересечения (если были) сохранены в векторе lines_set. Линии не связаны. Нужно их связать.
				*/
				if( facets_set->InterceptionLinesOnZPlane(z, precision, &lines_set) ){										
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
			float tmp_z;
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
		bool MakeSweepContours(float z){
			auto it_z = contours_by_z.find(z);
			if (it_z == contours_by_z.end())
				return false;
			std::cout<<"Start making sweep contours.."<<std::endl;
			std::vector<Point2D> wpts;
			std::set<id> ID_set_by_z = it_z->second;
			/*
			std::map<Point2D, id> points_heap; 
			std::cout<<"IDs: "<<std::endl;
			for(auto it_ID = ID_set_by_z.begin(); it_ID != ID_set_by_z.end(); it_ID++){
				if(equidistant_contours.find(*it_ID) == equidistant_contours.end())
					continue;
				GetWaypointsByID(*it_ID, wpts);
				std::cout<<*it_ID<<" waypoints: ";
				for(auto it_wpt = wpts.begin(); it_wpt != wpts.end(); it_wpt++)
					points_heap.emplace(std::make_pair(*it_wpt, *it_ID) );
				std::cout<<wpts.size()<<std::endl;
			}	*/
			//Now we have set of all points, 
			//this set is sorted by x (smallest to largest) and (if x1 == x2) by y (smallest to largest)
			
			std::vector<std::vector<Point2D>> upper_bounds, lower_bounds;
			std::vector<Border> borders;
			std::set<BorderLink> border_links_set;
			std::set<float> all_x;
			for(auto it : ID_set_by_z){
				std::cout<<"ID = "<<it<<std::endl;
				PrintContour(it);
				SplitContourIntoBorders(it, upper_bounds, lower_bounds);
				for(auto it : upper_bounds)
					borders.push_back(Border(true, it));
				for(auto it : lower_bounds)
					borders.push_back(Border(false, it));
		
				/*
				for(auto &it : borders)
					it.CreateInterpolatedLinksByX(all_x);
				
				/*
				for(auto &it : borders)
					it.GetLinks(border_links_set);
				//now we have fully sorted set of links, from smallest x to largest, and, if equal x, from largest y to smallest.
				//if x1 = x2 & y1 = y2, upper link will be smaller then lower link
				
				float current_x = border_links_set.begin()->x;
				for(auto it : border_links_set)
					it.PrintBorderLink();
				
				*/
				
				
				upper_bounds.clear();
				lower_bounds.clear();
			}
			std::cout<<"borders: "<<std::endl;
			for(auto &it : borders)
				it.PrintBorder();				
			for(auto &it : borders)
				it.ExtractAllXCoordinates(all_x);
			for(auto &it : all_x)
				std::cout<<it<<" ";
			return true;
		}
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
};





#endif
