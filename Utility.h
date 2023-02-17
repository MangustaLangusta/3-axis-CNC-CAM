#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include "BasicStructs.h"

struct SplitSettings{
	double z_min;
	double z_max;
	double spacing;
	bool whole_z_range;
	bool valid;
	SplitSettings(const double new_z_min, const double new_z_max, const double new_spacing);	// to split in certain z_range
	SplitSettings(const double new_spacing);	//to split whole area using new_spacing
	SplitSettings();
	void Initialize(const double new_z_min, const double new_z_max, const double new_spacing);
	void Initialize(const double new_spacing);
	bool IsValid();
	bool IsWholeZRange();
	~SplitSettings();
};

void Print(const Point3D &p);


#endif