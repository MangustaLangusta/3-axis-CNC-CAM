#ifndef RAW_STL
#define RAW_STL

struct STLHeader{
	char header[80];
	unsigned int number_of_triangles;
}

struct STLTriangle{
	float normal[3];	//in some stl files not initialized
	float v1[3];
	float v2[3];
	float v3[3];
	unsigned short attribute_byte_count;
};

class RawSTL{
	private:
		STLHeader header;
		std::vector<STLTriangle> triangles;
	public:
		RawSTL(const std::string filename){
			std::ifstream fin(filename);	//opened for reading
			fin.get(header);
			
			fin.close();
			
			std::string title, amount, st;
	std::ifstream fin("test.STL");	//Открыли для чтения
	ReadSymbols(80, &title, &fin);
	//ConsoleHex(title, 16);
	ReadSymbols(4, &amount, &fin);
	//std::cout<<"facets:"<<std::endl;
	for (int i = 0; i < GetFacetsAmount(&amount); i++){
		if (!ReadSymbols(50, &st, &fin))
			std::cout<<"end of file reached but not all facets have been read! "<<i<<std::endl;
		facets_st->push_back(st);
	}
	if(ReadSymbols(1, &st, &fin))
		std::cout<<"STL file successfully processed"<<std::endl;
	else 
		std::cout<<"STL file seems to be broken"<<std::endl;
	fin.close();
			
		}
}



#endif