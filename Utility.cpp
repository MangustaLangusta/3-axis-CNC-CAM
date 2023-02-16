#include "Utility.h"

SplitSettings::SplitSettings(const double new_z_min, const double new_z_max, const double new_spacing){
	z_min = new_z_min;
	z_max = new_z_max;
	spacing = new_spacing;
}

SplitSettings::SplitSettings(){}

SplitSettings::~SplitSettings(){}

void Print(const Point3D &p){	
	std::cout<<p.x<<" "<<p.y<<" "<<p.z<<std::endl; 
}