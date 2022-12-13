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
	public:
		Contour(){
			ID = last_ID++;					//update ID 
		}
		int GetID() { return ID; }
		static int GetLastID() { return last_ID; }
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
			if(waypoints[0] == waypoints[waypoints.size()-1])
				waypoints.pop_back();
			/*
				Вставить обработчик для случая, если контур незамкнут:
				else{
				
				{
			*/
		}
};

class EquidistantContour : public Contour{
	
};

class FieldContour : public Contour{
	public:
		FieldContour(){
			waypoints = FIELD_WAYPOINTS;
			z = FIELD_HEIGHT;
		}
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
		bool MakeEquidistantContours(){
			return true;
		}
};





#endif
