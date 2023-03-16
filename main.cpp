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

/*    local files   */

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



int main(int argc, char *argv[]){
	
	TaskManager main_task_manager(argc, argv);
	main_task_manager.StartTasksExecution();


	
	/*
	system("pause");
	system("cls");
	*/
	return 0;
}


/*
g++ -c main.cpp Facets.cpp TaskManager.cpp UserInterface.cpp BasicStructs.cpp Utility.cpp RawSTL.cpp Project.cpp Errors.cpp Contours.cpp
g++ main.o Facets.o TaskManager.o UserInterface.o BasicStructs.o Utility.o RawSTL.o Project.o Errors.o Contours.o



*/


	/*
	Line3D test_line1({0, 2, 5}, {3, 5, 5});
	Line3D test_line2({7, 2, 5}, {1, 5, 5});
	Point3D intersection_point;
	Matrix test_matrix({	{1, 2, -3, 4},
												{2, -1, 0, -5},
												{1, 0, -3, 0},
												{3, -2, 2, 1} });
												
	if(MathOperations::LimitedIntersectionOfTwoLines(test_line1, test_line2, &intersection_point))	
		Print(intersection_point);
	else 
		std::cout<<"not crossing"<<std::endl;
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