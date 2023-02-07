#include "RawSTL.h"

STLHeader::STLHeader(const char *header_buff){
	const int TITLE_SIZE = 80;																								
	const int NUMBER_OF_TRIANGLES_SIZE = 4;																		
	void *dest;																																
	const	void *src;																													
	dest = title;																															
	src = header_buff;																												
	memcpy(dest, src, TITLE_SIZE);																						
	dest = &number_of_triangles;																							
	src = header_buff + TITLE_SIZE;																						
	memcpy(dest, src, NUMBER_OF_TRIANGLES_SIZE);
}

STLHeader::~STLHeader(){}

STLTriangle::STLTriangle(const char *triangle_buff){																			
	const int NORMAL_ARRAY_SIZE = 4 * 3;	//as per STL specification					
	const int VERTEX_ARRAY_SIZE = 4 * 3;	//		-//-													
	const int ATTRIBUTE_SIZE = 2;					//		-//-													
	void *dest;																																
	const	void *src;																													
	if(sizeof(float) != 4)																										
		std::cerr<<"Error: size of float is not equal to 4!"<<std::endl;				
	dest = normal;																														
	src = triangle_buff;																											
	memcpy(dest, src, NORMAL_ARRAY_SIZE);																			
	dest = v1;																																
	src = triangle_buff + NORMAL_ARRAY_SIZE;																	
	memcpy(dest, src, VERTEX_ARRAY_SIZE);																			
	dest = v2;																																
	src = triangle_buff + NORMAL_ARRAY_SIZE + VERTEX_ARRAY_SIZE * 1;					
	memcpy(dest, src, VERTEX_ARRAY_SIZE);																			
	dest = v3;																																
	src = triangle_buff + NORMAL_ARRAY_SIZE + VERTEX_ARRAY_SIZE * 2;					
	memcpy(dest, src, VERTEX_ARRAY_SIZE);																			
	dest = &attribute_byte_count;																							
	src = triangle_buff + NORMAL_ARRAY_SIZE + VERTEX_ARRAY_SIZE * 3;					
	memcpy(dest, src, ATTRIBUTE_SIZE);																				
}	

STLTriangle::~STLTriangle(){}

Triangle3D STLTriangle::ConvertToTriangle(){
	Triangle3D result_triangle;
	result_triangle.vertex[0] = Point3D{v1[0], v1[1], v1[2]};
	result_triangle.vertex[1] = Point3D{v2[0], v2[1], v2[2]};
	result_triangle.vertex[2] = Point3D{v3[0], v3[1], v3[2]};
	return result_triangle;
}

RawSTL::RawSTL(const std::string &filename){
	if(GetDataFromBinarySTLFile(filename))
		valid = VALID;
	else 
		valid = NOT_VALID;
}

RawSTL::~RawSTL(){}

bool RawSTL::GetDataFromBinarySTLFile(const std::string &filename){
	std::ifstream fin(filename);			//file opened for reading
	/*	Declaring variables to be used */
	const int HEADER_SIZE = 84;
	const int TRIANGLE_SIZE = 50;
	char header_buff[HEADER_SIZE];
	char triangle_buff[TRIANGLE_SIZE];
	/*	Reading header	*/
	for(auto i = 0; i < HEADER_SIZE; i++)
		fin.get(header_buff[i]);
	if ( (fin.rdstate() & std::ifstream::eofbit ) != 0 ){
		std::cerr<<"Error: unexpected eof reached when reading STL file\n";
		fin.close();
		return false;
	}
	STLHeader header(header_buff);
	std::cout<<header.number_of_triangles<<std::endl;
	/* 	Header has been read successfully, 
			reading triangles...								*/
	for(long i = 0; i < header.number_of_triangles; i++){
		for(auto i = 0; i < TRIANGLE_SIZE; i++)
			fin.get(triangle_buff[i]);
		if ( (fin.rdstate() & std::ifstream::eofbit ) != 0 ){
			std::cerr<<"Error: unexpected eof reached when reading STL file"<<std::endl;
			triangles.clear();
			fin.close();
			return false;
		}
		triangles.emplace_back(STLTriangle(triangle_buff));
	}
	/*	Triangles extracted, 
			checking that eof reached		*/
	fin.get();
	if(!fin.eof()){
		std::cerr<<"Warning! All triangles extracted, but eof not reached!"<<std::endl;
		triangles.clear();
		fin.close();
		return false;
	}
	std::cout<<"STL file successfully processed"<<std::endl;
	fin.close();
	return true;
}


bool RawSTL::IsValid(){ 
	if(valid == VALID)
		return true;
	return false;
}

const std::vector<Triangle3D> RawSTL::GetTriangles3D() { 
	std::vector<Triangle3D> result_triangles;
	for(auto &it : triangles)
		result_triangles.emplace_back(it.ConvertToTriangle());
	return result_triangles;
}