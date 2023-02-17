#include "Utility.h"

SplitSettings::SplitSettings(const double new_z_min, const double new_z_max, const double new_spacing){
	z_min = new_z_min;
	z_max = new_z_max;
	spacing = new_spacing;
	whole_z_range = false;
	valid = true;
}

SplitSettings::SplitSettings(const double new_spacing){
		spacing = new_spacing;
		whole_z_range = true;
		valid = true;
}

SplitSettings::SplitSettings(){
	valid = false;
}

SplitSettings::~SplitSettings(){}

void SplitSettings::Initialize(const double new_z_min, const double new_z_max, const double new_spacing){
	z_min = new_z_min;
	z_max = new_z_max;
	spacing = new_spacing;
	whole_z_range = false;
	valid = true;
}

void SplitSettings::Initialize(const double new_spacing){
	spacing = new_spacing;
	whole_z_range = true;
}

bool SplitSettings::IsValid(){
	return valid;
}

bool SplitSettings::IsWholeZRange(){
	return whole_z_range;
}

void Print(const Point3D &p){	
	std::cout<<p.x<<" "<<p.y<<" "<<p.z<<std::endl; 
}