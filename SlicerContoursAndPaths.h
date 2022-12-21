#ifndef SLICER_CONTOURS_AND_PATHS_H
#define SLICER_CONTOURS_AND_PATHS_H

#define FIELD_WAYPOINTS { {-10, -10}, {-10, 110}, {110, 110}, {110, -10} }
#define FIELD_HEIGHT 150

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
		OffsetSide side = not_defined;
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
		Contour(Contour* parent, OffsetSide init_side, Offset init_offset, id &assigned_ID, float &z_plane){
			ID = next_ID++;					//update ID 
			parent_ID = parent->GetID();
			z = parent->GetZ();
			std::vector<Point2D> base_waypoints = parent->GetWaypoints();
			side = init_side;
			offset = init_offset;
			assigned_ID = ID;
			z_plane = z;
			long wpts_amount = base_waypoints.size();
			long target = 0;
			long prev = wpts_amount - 1;
			long next = 1;
			Line2D base_line_a = {base_waypoints[prev], base_waypoints[target]};
			Line2D base_line_b = {base_waypoints[target], base_waypoints[next]};
			Line2D equidist_line_a = DrawEquidistantLine(base_line_a, offset, side);
			Line2D equidist_line_b = DrawEquidistantLine(base_line_b, offset, side);
			Line2D bissectris, bissectris_normal;
			Point2D equidist_lines_intercept, bissectris_intercept, bissectris_normal_b, wpt_a, wpt_b;
			float ratio;
			do {
				//0. Проверяем угол, который обводим (острый, тупой или развернутый). 
				if(Lines2DIntercept(equidist_line_a, equidist_line_b, equidist_lines_intercept)){
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
				prev = target;
				target = next;
				next = (next + 1) % wpts_amount;
				base_line_b = {base_waypoints[target], base_waypoints[next]};
				equidist_line_a = equidist_line_b;
				equidist_line_b = DrawEquidistantLine(base_line_b, offset, side);
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
		id GetID() { return ID; }
		ContourType GetType() { return type; }
		static id GetNextID() { return next_ID; }
		float GetZ() { return z; }
		bool IsLoop() { return loop; }
		std::vector<Point2D> GetWaypoints() { return waypoints; }
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
		//Contour field(field, FIELD_WAYPOINTS, FIELD_HEIGHT);						//one field for all objects
		std::map<id, Contour> all_contours;															//all contours are stored here
		std::set<id> raw_contours;																		//all IDs of raw contours 
		std::set<id> equidistant_contours;														//all IDs of equidistant contours
		std::map<id, std::set<id>> contours_tree;										//jerarhy between contours
		std::map<float, std::set<id>> contours_by_z;									//IDs sorted by z-coordinate
		
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
		void PrintContour(id request){
			if (GetContourByID(request))
				GetContourByID(request)->PrintContour();
			else 
				std::cout<<"Attempt to print contour with ID "<<request<<" failed: Contour does not exist"<<std::endl;
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
			std::map<float, std::set<id>>::iterator it_z = contours_by_z.find(z);
			if (it_z == contours_by_z.end())
				it_z = contours_by_z.insert( std::make_pair(z, std::set<id>{}) ).first;
			while(!lines_association.empty()){
				tmp_ID = Contour::GetNextID();
				all_contours.emplace( std::make_pair(tmp_ID, Contour(&lines_association, z)) );
				raw_contours.insert(tmp_ID);
				it_z->second.insert(tmp_ID);
				PrintContour(tmp_ID);
			}
			return true;
		}
////////////////////////////////////////////////////////////////////////////////////////////
	public:
		void SetZStep(float new_z_step_mm) { z_step_mm = new_z_step_mm; }
		void SetZOffset(float new_z_offset_mm) { z_offset_mm = new_z_offset_mm; }
		void SetPrecision(float new_precision_mm) { precision = new_precision_mm; }
		
		std::set<id> GetAllRawContoursIDs() { return raw_contours; }
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
			return true;
		}
///////////////////////////////////////////////////////////////////////////////////////////
		bool MakeEquidistantContours(Offset offset, OffsetSide off_side, std::set<id> request_ids){
			std::cout<<"Making euidistant contours.."<<std::endl;
			/*
			id tmp_ID;
			float tmp_z;
			Contour *parent_contour;
			for (auto it = request_ids.begin(); it != reqiest_ids.end(); it++){
				parent_contour = 
				
			}
			*/
			std::cout<<"Equidistant contours done!"<<std::endl;
			return true;
		}
};





#endif
