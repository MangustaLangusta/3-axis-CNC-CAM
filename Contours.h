#ifndef CONTOURS_H
#define CONTOURS_H

#include <list>
#include <vector>
#include <map>
#inclued <set>
#include "BasicStructs.h"
#include "Errors.h"

class Contour{
	private:
		std::list<Point3D> waypoints;
	public:
		Contour(const std::list<Point3D> new_waypoints, std::list<Error>* errors_list, ErrorFlag* error_flag);	//TO MAKE
		~Contour();
		double GetZPlane(bool *valid);
		
};


class ContoursAggregator{
	private:
		std::map<double, std::set<Contour*>> raw_contours;					//source contours from facets
		std::map<double, std::set<Contour*>> outer_contours;				//contours on outer layers, to be check for crossing with new contours
		std::map<double, std::set<Contour*>> contours_ready_to_make_path;	//contours in inner layers, no need to check for crossing with new contours
		
		
		
	public:
		ContoursAggregator(const std::vector<std::list<Point3D>> source_contours, std::list<Error> *errors_list, ErrorFlag *error_flag);	//TO MAKE
		~ContoursAggregator();
		
		
		
		std::vector<Contour*> GetPreparedContours(const double z_plane);	//TO MAKE FUNCTION
		
};


#endif