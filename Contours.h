#ifndef CONTOURS_H
#define CONTOURS_H

#include <list>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include "BasicStructs.h"
#include "Errors.h"
#include "Utility.h"

#define TEST_WORKFIELD_WAYPOINTS { Point3D{-10, -10, 0}, Point3D{110, -10, 0}, Point3D{110, 110, 0}, Point3D{-10, 110, 0} } 
#define TEST_WORKFIELD_Z_EXTREMUMS {0, 60}

class Contour{
	private:
		std::list<Point3D> waypoints;
		double z_plane;
		bool valid;
		
		void RemoveExessivePoints();		//removes inner points laying on same line
		
	public:
		Contour(const std::list<Point3D> &new_waypoints, ErrorsLog* errors_log);		//without self-crossing check
		Contour(const std::list<Point3D> &new_waypoints, std::set<Contour*> *contours_aparted, ErrorsLog* errors_log);	//with self-crossing check
		~Contour();
		double GetZPlane() const;
		std::list<Point3D> GetWaypoints(ErrorsLog *errors_log) const;
		
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
		bool GenerateFieldContour(const double &z_plane, ErrorsLog *errors_log, Contour **result_contour);
};

class ContoursAggregator{
	private:
		std::map<double, std::set<Contour*>> raw_contours;					//source contours from facets
		std::map<double, std::set<Contour*>> path_pattern;	//contours in inner layers, no need to check for crossing with new contours
		
		WorkField *workfield;
		
		//void MakeContoursPatern(const double &z_plane, ErrorsLog *errors_log);	//TO MAKE FUNCTION
		
		std::list<Point3D> GenerateEquidistantContourFragment(const Point3D &prev_wpt, const Point3D &current_wpt, const Point3D &next_wpt);

		
		bool Equidistant(const Contour* source_contour, const double &spacing, Contour* new_equidistant_contour);
		bool EquidistantSinglePointContour(const Point3D &single_wpt, const double &spacing, Contour* equidistant_contour);
		//bool Merge(const Contour* contour_to_be_merged, std::set<Contour*> active_contours);
		
	public:
		ContoursAggregator(const std::vector<std::list<Point3D>> source_contours, ErrorsLog *errors_log);
		~ContoursAggregator();
		
		bool AssignWorkField(WorkField *new_workfield);
		
		std::vector<Contour*> GetPreparedContours(const double &spacing,  ErrorsLog *errors_log);	//TO MAKE FUNCTION
		
};

WorkField* CreateTestWorkField();



#endif