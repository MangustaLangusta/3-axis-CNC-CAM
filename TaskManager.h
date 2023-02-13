#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <list>
#include <vector>
#include <iostream>
#include <assert.h>
#include "UserInterface.h"
#include "Project.h"
#include "Errors.h"
#include "RawSTL.h"


/*
	Task Manager is the first object to be created. 
	If additional arguments were passed via command line, they will be transformed to task in function CommandLineArgumentsToTasks()
	After creation need to initiate Tasks processing by calling function StartTasksExecution()
*/

/*
	Forward declarations//////////////////
*/

class Task;
class TaskManager;
class UserInterface;
class ConsoleUserInterface;
class Project;

/*
/////////////////////////////////////////
*/
using RequestCode = long;

class Task{
	private:
		
	public:
		TaskManager* assigned_task_manager;
		~Task();
		Task(TaskManager* new_assigned_task_manager);
		void Message(const std::string text);
		void ErrorMessage(const std::string text, const Error error);
		virtual void Execute() = 0;
};

class TaskCreateNewProject : public Task{
	private:
		std::string project_name;
	public:
		TaskCreateNewProject(TaskManager* new_assigned_task_manager, std::string new_project_name);
		~TaskCreateNewProject();
		void Execute();	
};

class TaskEmergencyStop : public Task{
	public:
		TaskEmergencyStop(TaskManager* new_assigned_task_manager);
		~TaskEmergencyStop();
		void Execute();
};

class TaskProcessInputFile : public Task{
	private: 
		std::string filename;
	public:
		TaskProcessInputFile(TaskManager* new_assigned_task_manager, std::string new_filename);
		~TaskProcessInputFile();
		void Execute();
};

class TaskManager{
	private:
		std::list<Task*> tasks;
		UserInterface* user_interface;
		Project* assigned_project;
		bool execution_permitted;
		void CommandLineArgumentsToTasks(int argc, char *argv[]);
		void ExecuteNextTask();
		bool IsExecutionPermitted() const;
		void StartConsoleUserInterface();
	public:
		TaskManager(int argc, char *argv[]);
		~TaskManager();
		void StartTasksExecution();
		void ProhibitTasksExecution();
		void PermitTasksExecution();
		UserInterface* GetUserInterface() const;
		void AssignProject(Project* new_project);
		Project* GetAssignedProject();
		void MessageToUserInterface(const std::string text);
		void ErrorMessageToUserInterface(const std::string text, const Error error);
			//Requests from other parts of program
		void Request(RequestCode request_code);										//If no additional info required
		void Request(RequestCode request_code, std::string str);	//if text info required
		void RequestEmergencyStop();
		void RequestToCreateNewProject(std::string project_name);
		void RequestToMakeGCodeFromFile(std::string filename);	//for quick start with pre-defined settings
		void RequestToProcessInputFile(std::string filename);	//output - composite facet body
		
};




#endif