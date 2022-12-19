#ifndef SLICER_CONTOURS_AND_PATHS_H
#define SLICER_CONTOURS_AND_PATHS_H

#define FIELD_WAYPOINTS { {-10, -10}, {-10, 110}, {110, 110}, {110, -10} }
#define FIELD_HEIGHT 150


class Contour{
	protected:
		long long ID;
		static long long last_ID;
		std::vector<Point2D> waypoints;
		float z;
		bool loop;
	public:
		Contour(){
			ID = last_ID++;					//update ID 
		}
		int GetID() { return ID; }
		static int GetLastID() { return last_ID; }
		float GetZ() { return z; }
		bool IsLoop() { return loop; }
		std::vector<Point2D> GetWaypoints() { return waypoints; }
		void PrintContour(){
			std::cout<<"Contour z = "<<z<<" ID = "<<ID<<std::endl;
			for(auto it = waypoints.begin(); it != waypoints.end(); it++)
				it->print_point();	
		}
};

class RawContour : public Contour{
	public:
		RawContour(std::multimap <Point2D, Point2D> *lines_association, float z_plane){
			z = z_plane;
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
};

class EquidistantContour : public Contour{
	private: 
		long long parent_ID;
		float distance;
		bool side;
	public: 
		EquidistantContour(Contour *parent, bool init_side, float init_distance) {
			parent_ID = parent->GetID();
			z = parent->GetZ();
			std::vector<Point2D> base_waypoints = parent->GetWaypoints();
			side = init_side;
			distance = init_distance;
			long wpts_amount = base_waypoints.size();
			long target = 0;
			long prev = wpts_amount - 1;
			long next = 1;
			Line2D base_line_a = {base_waypoints[prev], base_waypoints[target]};
			Line2D base_line_b = {base_waypoints[target], base_waypoints[next]};
			Line2D equidist_line_a = DrawEquidistantLine(base_line_a, distance, side);
			Line2D equidist_line_b = DrawEquidistantLine(base_line_b, distance, side);
			Line2D bissectris, bissectris_normal;
			Point2D equidist_lines_intercept, bissectris_intercept, bissectris_normal_b, wpt_a, wpt_b;
			float ratio;
			do {
				//0. Проверяем угол, который обводим (острый, тупой или развернутый). 
				if(Lines2DIntercept(equidist_line_a, equidist_line_b, equidist_lines_intercept)){
					bissectris = {base_waypoints[target], equidist_lines_intercept};
					ratio = distance / bissectris.GetLength();
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
				equidist_line_b = DrawEquidistantLine(base_line_b, distance, side);
			}while(target != 0);
		}
};

class FieldContour : public Contour{
	public:
		FieldContour(){
			waypoints = FIELD_WAYPOINTS;
			z = FIELD_HEIGHT;
		}
};

class SweepContour : public Contour{
	public:
		
};

long long Contour::last_ID = 0;


class Path {
	private:
		Instrument used_instrument;
		float speed;
		
	public:
		
};



class ContoursAndPaths {
	private:
		static FieldContour field;																						//one field for all objects
		std::map <float, std::map <long long, RawContour> > raw_contours;			//(float) z, (map (int) ID, (RawContour) contours) 
		std::map <float, std::map <long long, EquidistantContour> > contours;
		std::map <long long, std::vector<long long> > contours_tree;											//jerarhy between contours
		
		float z_step_mm;
		float z_offset_mm;
		float precision;
////////////////////////////////////////////////////////////////////////////////////////////
		bool ExtractRawContoursFromLinesSet(std::vector<Line2D> *lines_set, float z){
			std::multimap <Point2D, Point2D> lines_association;
			for(auto it = lines_set->begin(); it != lines_set->end(); it++){
				lines_association.insert(std::pair<Point2D, Point2D> (it->a, it->b) );
			}
			//std::cout<<"Lines association done! Size: "<<lines_association.size()<<std::endl;
			if(lines_association.empty())
				return false;
				//For each z will be new record in "contours"
			std::map <float, std::map <long long, RawContour>>::iterator contours_iter;
			std::map <long long, RawContour> m1 = {};
			contours_iter = raw_contours.insert( std::pair <float, std::map <long long, RawContour>> (z, m1) ).first;
			while(!lines_association.empty()){
				contours_iter->second.insert(std::make_pair(Contour::GetLastID(), RawContour(&lines_association, z)));
			}
			for(auto it = raw_contours.begin()->second.begin(); it != raw_contours.begin()->second.end(); it++){
				it->second.PrintContour();
			}
			
			/*
			Line2D test_line = { (Point2D) {0,0}, (Point2D) {0,10} };
			Line2D equidist_test = DrawEquidistantLine(test_line, 10, true);
			equidist_test.print_line();*/
			
			return true;
		}
////////////////////////////////////////////////////////////////////////////////////////////
	public:
		void SetZStep(float new_z_step_mm) { z_step_mm = new_z_step_mm; }
		void SetZOffset(float new_z_offset_mm) { z_offset_mm = new_z_offset_mm; }
		void SetPrecision(float new_precision_mm) { precision = new_precision_mm; }
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
		bool MakeEquidistantContours(float distance){
			std::map <float, std::map <long long, EquidistantContour>>::iterator contours_iter;
			std::map <long long, EquidistantContour> eq_cont = {};
			float z;
			for(auto it_z = raw_contours.begin(); it_z != raw_contours.end(); it_z++){
				z = it_z->first;
				for(auto it_id = it_z->begin(); it_id != it_z->end(); it_id++){
					contours_iter = contours.insert( std::pair <float, std::map <long long, EquidistantContour>> (z, eq_cont) ).first;
				}
			}
			return true;
		}
};





#endif
