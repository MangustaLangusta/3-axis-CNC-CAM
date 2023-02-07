#ifndef RAW_STL
#define RAW_STL

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "BasicStructs.h"

struct STLHeader{
	char title[80];
	unsigned int number_of_triangles;
	STLHeader(const char *header_buff);
	~STLHeader();
};																																						
																																							
struct STLTriangle{																														
	float normal[3];	//in some stl files not initialized												
	float v1[3];																																
	float v2[3];																																
	float v3[3];																																
	unsigned short attribute_byte_count;																												
	STLTriangle(const char *triangle_buff);
	~STLTriangle();
	Triangle3D ConvertToTriangle();
};

/*
	Contains only info, extracted from .stl file - no additional modyfing applied
	valid is set to VALID if no issues during processing stl file
*/
class RawSTL{
	private:
		std::vector<STLTriangle> triangles;
		Validity valid;	
		bool GetDataFromBinarySTLFile(const std::string &filename);
	public:
		RawSTL(const std::string &filename);
		~RawSTL();
		bool IsValid();
		const std::vector<Triangle3D> GetTriangles3D();
};

#endif