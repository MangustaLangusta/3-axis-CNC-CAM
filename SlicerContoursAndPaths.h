#ifndef SLICER_CONTOURS_AND_PATHS_H
#define SLICER_CONTOURS_AND_PATHS_H

class Contour{
	private:
		int ID;
		static int last_ID;
		std::vector<Point2D> waypoints;
		float z;
	public:
		Contour(std::multimap <Point2D, Point2D> *lines_association, float z_plane){
			//std::cout<<"Contour initialisation z = "<<z_plane<<std::endl;
			
			
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
			ID = last_ID++;					//update ID in the end
		}
		int GetID() { return ID; }
		static int GetLastID() { return last_ID; }
		void PrintContour(){
			std::cout<<"Contour z = "<<z<<std::endl;
			for(auto it = waypoints.begin(); it != waypoints.end(); it++)
				it->print_point();	
		}
};
int Contour::last_ID = 0;


class Path {
	private:
		Instrument used_instrument;
		float speed;
		
	public:
		
};



class ContoursAndPaths {
	private:
		
		std::map <float, std::map <int, Contour> > contours;			//(float) z, (map (int) ID, (Contour) contours) all contours - raw and equidistant
		
		
	//	std::multimap <float, std::vector<Contour> > raw_contours;	//каждому z соответствует свой набор контуров
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
			std::map <float, std::map <int, Contour>>::iterator contours_iter;
			std::map <int, Contour> m1 = {};
			contours_iter = contours.insert( std::pair <float, std::map <int, Contour>> (z, m1) ).first;
			while(!lines_association.empty()){
				contours_iter->second.insert(std::make_pair(Contour::GetLastID(), Contour(&lines_association, z)));
				std::cout<<"ID = "<<Contour::GetLastID()<<std::endl;
				//contours_iter->second.find(Contour::GetLastID()-1)->second.PrintContour();
			}
			for(auto it = contours.begin()->second.begin(); it != contours.begin()->second.end(); it++){
				std::cout<<"Check ID "<<it->first<<std::endl;
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

};





#endif
