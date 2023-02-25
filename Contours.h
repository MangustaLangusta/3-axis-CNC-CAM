#ifndef CONTOURS_H
#define CONTOURS_H

#include <list>
#include <vector>
#include <map>
#include <set>
#include "BasicStructs.h"
#include "Errors.h"
#include "Utility.h"



class Contour{
	private:
		std::list<Point3D> waypoints;
		double z_plane;
		bool valid;
		
		void RemoveExessivePoints();		//removes inner points laying on same line
		
	public:
		Contour(const std::list<Point3D> new_waypoints, std::list<Error>* errors_list, ErrorFlag* error_flag);	//TO MAKE
		~Contour();
		double GetZPlane() const;
		
		bool IsValid() const;
		void PrintContour() const;
};

class WorkField{
	private:
		std::pair<double, double> z_extremums;
		Contour* base_contour;
		bool valid;
	public:
		WorkField(const std::pair<double, double> &new_z_extremums, Contour* new_base_contour, ErrorsLog *errors_log);
		~WorkField();
		bool IsValid() const;
};

class ContoursAggregator{
	private:
		std::map<double, std::set<Contour*>> raw_contours;					//source contours from facets
		std::map<double, std::set<Contour*>> outer_contours;				//contours on outer layers, to be check for crossing with new contours
		std::map<double, std::set<Contour*>> contours_ready_to_make_path;	//contours in inner layers, no need to check for crossing with new contours
		
		WorkField *workfield;
		
		void MakeContoursPatern(const double &z_plane, ErrorsLog *errors_log);	//TO MAKE FUNCTION
		
	public:
		ContoursAggregator(const std::vector<std::list<Point3D>> source_contours, std::list<Error> *errors_list, ErrorFlag *error_flag);	//TO MAKE
		~ContoursAggregator();
		
		void AssignWorkField(WorkField *new_workfield, ErrorsLog *errors_log);
		
		std::vector<Contour*> GetPreparedContours(const double &spacing,  ErrorsLog *errors_log);	//TO MAKE FUNCTION
		
};


#endif