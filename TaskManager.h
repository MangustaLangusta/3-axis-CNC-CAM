#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <list>
#include <vector>
#include <iostream>
#include "UserInterface.h"
#include "Project.h"
#include "ErrorCodes.h"

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

class Task{
	private:
		
	public:
		TaskManager* assigned_task_manager;
		virtual void Execute() = 0;
		~Task();
		Task(TaskManager* new_assigned_task_manager);
};

class TaskRunConsoleUserInterface : public Task{
	public:
		TaskRunConsoleUserInterface(TaskManager* new_assigned_task_manager);
		~TaskRunConsoleUserInterface();
		void Execute() override;

};

class TaskInitiateConsoleUserInterface : public Task{
	public:
		TaskInitiateConsoleUserInterface(TaskManager* new_assigned_task_manager);
		~TaskInitiateConsoleUserInterface();
		void Execute() override;
};

class TaskCreateNewProject : public Task{
	private:
		std::string project_name;
	public:
		TaskCreateNewProject(TaskManager* new_assigned_task_manager, std::string new_project_name);
		~TaskCreateNewProject();
		void Execute() override;	
};

class TaskEmergencyStop : public Task{
	public:
		TaskEmergencyStop(TaskManager* new_assigned_task_manager);
		~TaskEmergencyStop();
		void Execute() override;
};


class TaskManager{
	private:
		std::list<Task*> tasks;
		UserInterface* user_interface;
		Project* assigned_project;
		bool execution_permitted;
		void CommandLineArgumentsToTasks(int argc, char *argv[]);
		void ExecuteNextTask();
		bool HaveTasksToDo()const;
		bool IsExecutionPermitted() const;
	public:
		TaskManager(int argc, char *argv[]);
		~TaskManager();
		void StartTasksExecution();
		void ProhibitTasksExecution();
		void PermitTasksExecution();
		void AssignUserInterface(UserInterface* new_user_interface);
		UserInterface* GetUserInterface() const;
		void AssignProject(Project* new_project);
		void MessageToUserInterface(std::string text);
		void ErrorMessageToUserInterface(std::string text, ErrorCode error_code);
			//Requests from other parts of program
		void RequestEmergencyStop();
		void RequestToInitiateConsoleUserInterface();
		void RequestToRunConsoleUserInterface();
		void RequestToCreateNewProject(std::string project_name);
		void RequestToMakeGCodeFromFile(std::string filename);	//for quick start with pre-defined settings
		void RequestToProcessInputFile(std::string filename);	//output - composite facet body
		
};




#endif