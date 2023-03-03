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
/*
#include "BasicStructs.h"
#include "Utility.h"
#include "RawSTL.h"
#include "Facets.h"
#include "Contours.h"
#include "Config.h"
#include "UserInterface.h"
#include "TaskManager.h"
#include "Project.h"
*/


#define DEFAULT_INPUT_FILE_NAME "test.stl"
#define DEFAULT_LOG_FILE_NAME "log.txt"


using namespace std;


void sysout(double **a, double *y, int n)
{
  for (int i = 0; i < n; i++) 
  {
    for (int j = 0; j < n; j++) 
    {
      cout << a[i][j] << "*x" << j;
      if (j < n - 1)
        cout << " + ";
    }
    cout << " = " << y[i] << endl;
  }
  return;
}

int main() 
{
  double **a, *y, *x;
  int n;
  system("cls");
  cout << "Now many equasions?: ";
  cin >> n;
  a = new double*[n];
  y = new double[n];
  for (int i = 0; i < n; i++) 
  {
    a[i] = new double[n];
    for (int j = 0; j < n; j++) 
    {
      cout << "a[" << i << "][" << j << "]= ";
      cin >> a[i][j];
    }
  }
  for (int i = 0; i < n; i++) 
  {
    cout << "y[" << i << "]= ";
    cin >> y[i];
  }
  sysout(a, y, n);
  x = gauss(a, y, n);
  for (int i = 0; i < n; i++) 
    cout << "x[" << i << "]=" << x[i] << endl;
  cin.get(); cin.get();
  return 0;
}

/*

int main(int argc, char *argv[]){
	TaskManager main_task_manager(argc, argv);
	main_task_manager.StartTasksExecution();


	
	/*
	system("pause");
	system("cls");
	return 0;
}


/*
g++ -c main.cpp Facets.cpp TaskManager.cpp UserInterface.cpp BasicStructs.cpp Utility.cpp RawSTL.cpp Project.cpp Errors.cpp Contours.cpp
g++ main.o Facets.o TaskManager.o UserInterface.o BasicStructs.o Utility.o RawSTL.o Project.o Errors.o Contours.o



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