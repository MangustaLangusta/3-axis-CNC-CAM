#ifndef CONTOURS_H
#define CONTOURS_H

class Contour{
	private:
		std::list<Point3D> waypoints;
	public:
		Contour(std::list<Point3D> new_waypoints);
};

class ContoursAggregator{
	private:
		std::vector<Contour*> contours;
		
	public:
		ContoursAggregator(std::vector<Line3D> source_lines);
	
};

Contour::Contour(std::list<Point3D> new_waypoints){
	std::cout<<"some code for COntour creation"<<std::endl;
};


ContoursAggregator::ContoursAggregator(std::vector<Line3D> source_lines){
	std::cout<<"Some code for creating ContoursAggregator class"<<std::endl;
	
}



#endif