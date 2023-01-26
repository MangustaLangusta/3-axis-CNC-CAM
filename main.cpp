#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cmath>
#include <vector>
#include <list>
#include <set>
#include <algorithm>
#include <map>

#include "SlicerUtilityStructs.h"
#include "SlicerFacets.h"
#include "SlicerInstruments.h"
#include "SlicerContoursAndPaths.h"






/*
Преобразует 4 символа в число в порядке от младшего байта к старшему
Возвращает получившееся число - количество полигонов
*/
int GetFacetsAmount(std::string *s){
	int result = 0;
	for(int i = 0; i < 4; i++){
		result += ((*s)[i] << i);
	}
	return result;
}

/*
Преобразует строку символов в объект класса facet, который и возвращает
Point with max z coordinate becomes first point in facet. General order of points remains the same
(rule of right hand: 1-2-3 or 2-3-1 or 3-1-2)
*/
Facet GetFacet(std::string s){
	Facet result;
	PreciseDekartCoords point_coord;
	int count_axis = 0;
	int count_points = 0;
	int count_bytes = 0;
	float point[3];
	char c[4];
	void *pch_src, *pf_dest;
	void *pt = &c;
	double max_z;
	double min_z;
	int max_z_point_number = 1;
	for (auto it = s.begin(); it != s.end(); it++){
		c[count_bytes] = *it;
		if (count_bytes == 4){
			count_bytes = 0;
			pch_src = c;
			pf_dest = &point[count_axis];
			memcpy(pf_dest, pch_src, 4);
			count_axis++;
			if(count_axis == 3){
				std::cout<<point[0]<<" "<<point[1]<<" "<<point[2]<<std::endl;
				point_coord.MakeCoords(point);
				count_axis = 0;
				switch(count_points){
					case 0: 
						result.normal = point_coord;
						break;
					case 1: 
						result.vertex_vector[0] = point_coord;
						break;
					case 2: 
						result.vertex_vector[1] = point_coord;
						break;
					case 3: 
						result.vertex_vector[2] = point_coord;
						break;
					default: 
						break;
				}
				count_points++;
			}
		}
		count_bytes++;
	}
	result.PrintFacet();
	result.max_z = result.vertex_vector[0].z;
	result.min_z = result.vertex_vector[0].z;
	for (int i = 1; i < 3; i++){
		if(result.vertex_vector[i].z > result.max_z)
			result.max_z = result.vertex_vector[i].z;
		if(result.vertex_vector[i].z < result.min_z)
			result.min_z = result.vertex_vector[i].z;
	}
	return result;
}

/*
Читает символы из файла fin в количестве len и записывает по адресу s
Возвращает true если не был достигнут конец файла
Иначе возвращает false. По адресу s содержится то, что было прочитано в любом случае
*/
bool ReadSymbols(int len, std::string *s, std::ifstream *fin){
	s->clear();
	char ch;
	for(int i = 0; i < len; i++){
		if(fin->eof())
			return false;
		fin->get(ch);
		*s = *s + ch;
	}
	return true;
}	


/*
Processes binary STL file "file_name" and extracts information about facets in vector of string on 
address facets_st. Each string in this vector have size of 50 bytes and corresponds 
to each facet an accordance with specification of STL file
*/
void ProcessStlFile(std::string file_name, std::vector<std::string> *facets_st){
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

/*
Creates formalized vector of facets in FacetsSet object
*/
void CreateFacetsVector(std::string input_file_name, FacetsSet *facets_set){
	std::vector<std::string> facets_st;
	std::vector<Facet> facets_vector;
	ProcessStlFile(input_file_name, &facets_st);
	for(auto it = facets_st.begin(); it != facets_st.end(); it++){
		facets_set->AddFacet(GetFacet(*it));
	}
	facets_set->SortTopToBottom();
	return;
}

Instrument test_instrument = Instrument("test_instrument", 8, 0,0,0);		//for testing

/*

*/
void CreateGCode(std::string input_file_name){
	FacetsSet facets_set;
	InstrumentsSet instruments;	
	ContoursAndPaths contours_and_paths;
	contours_and_paths.SetZStep(5);
	contours_and_paths.SetZOffset(1);
	CreateFacetsVector(input_file_name, &facets_set);
	contours_and_paths.MakeRawContours(&facets_set);
	return;
	contours_and_paths.MakeEquidistantContours(10, contours_and_paths.GetAllRawContoursIDs());
	contours_and_paths.PrintAllContours();
	contours_and_paths.MakeSweepContours(45);	//for testing purposes
	contours_and_paths.MakePathsFromAllSweepContours(test_instrument);
	
	/*
	contours_and_paths.MakeTestContours();
	contours_and_paths.MakeSweepContours(test::test_z);
	contours_and_paths.MakePathsFromAllSweepContours(test::test_instrument);
	*/
}


int main(){
		//LATER TO MAKE SPECIAL FUNCTION FOR THIS ****************************
	Coordinate::SetPrecision(static_cast<Precision>(DEFAULT_PRECISION));
		//********************************************************************
	CreateGCode("test.STL");
	
	/*
	std::string title, amount, facets;
	std::ifstream fin("test.STL");	//Открыли для чтения
	ReadSymbols(80, &title, &fin);
	ConsoleHex(title, 16);
	ReadSymbols(4, &amount, &fin);
	std::cout<<"facets:"<<std::endl;
	for (int i = 0; i < GetFacetsAmount(&amount); i++){
		facets.clear();
		if (!ReadSymbols(50, &facets, &fin))
			std::cout<<"eof! "<<i<<std::endl;
		GetFacet(facets, i);
	}
	if(ReadSymbols(1, &facets, &fin))
		std::cout<<"finish. eof"<<std::endl;
	

	fin.close();	*/
	system("pause");
	system("cls");
	return 0;
}




/*
class owl_man {} Yoshi;
class owl_woman {} Coolya;
pair <owl_man, owl_woman> lovely_family;
lovely_family = make_pair(Yoshi, Coolya);
*/





/* first facet
0  0  80 bf
0  0  0  0
31 31 d  25
dd 9c dc 27
0  0  20 42
0  0  48 42
0  0  0  0
0  0  20 42
0  0  0  0
dd 9c dc 27
0  0  0  0
0  0  48 42
0  0
*/