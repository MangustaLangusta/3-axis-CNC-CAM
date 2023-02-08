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

/*    NEW CLASSES ARE BELOW   */
#include "BasicStructs.h"
#include "Utility.h"
#include "RawSTL.h"
#include "Facets.h"
#include "Contours.h"
#include "Config.h"
#include "UserInterface.h"
#include "TaskManager.h"
#include "Project.h"



#define DEFAULT_INPUT_FILE_NAME "test.stl"
#define DEFAULT_LOG_FILE_NAME "log.txt"


/*
void CreateGCode(std::string input_file_name){
	FacetsSet facets_set;
	InstrumentsSet instruments;	
	ContoursAndPaths contours_and_paths;
	contours_and_paths.SetZStep(5);
	contours_and_paths.SetZOffset(1);
	CreateFacetsVector(input_file_name, &facets_set);
	contours_and_paths.MakeRawContours(&facets_set);
	contours_and_paths.MakeEquidistantContours(10, contours_and_paths.GetAllRawContoursIDs());
	contours_and_paths.PrintAllContours();
	contours_and_paths.MakeSweepContours(45);	//for testing purposes
	contours_and_paths.MakePathsFromAllSweepContours(test_instrument);
	
	/*
	contours_and_paths.MakeTestContours();
	contours_and_paths.MakeSweepContours(test::test_z);
	contours_and_paths.MakePathsFromAllSweepContours(test::test_instrument);
	
}
*/

int main(int argc, char *argv[]){
	TaskManager main_task_manager(argc, argv);
	main_task_manager.StartTasksExecution();
	/*
	RawSTL raw_stl(DEFAULT_INPUT_FILE_NAME);
	CompositeFacetBody composite_facet_body(raw_stl.GetTriangles3D());
	*/
	
	/*
		//LATER TO MAKE SPECIAL FUNCTION FOR THIS ****************************
	Coordinate::SetPrecision(static_cast<Precision>(DEFAULT_PRECISION));
		//********************************************************************
	CreateGCode("test.STL");
	
	
	*/
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
g++ -c main.cpp Facets.cpp TaskManager.cpp UserInterface.cpp BasicStructs.cpp Utility.cpp RawSTL.cpp Project.cpp ErrorCodes.cpp
g++ main.o Facets.o TaskManager.o UserInterface.o BasicStructs.o Utility.o RawSTL.o Project.o ErrorCodes.o



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