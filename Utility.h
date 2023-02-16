#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include "BasicStructs.h"

struct SplitSettings{
	double z_min;
	double z_max;
	double spacing;
	SplitSettings(const double new_z_min, const double new_z_max, const double new_spacing);
	SplitSettings();
	~SplitSettings();
};

void Print(const Point3D &p);


#endif