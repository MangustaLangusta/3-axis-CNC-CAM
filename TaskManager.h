#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <list>
#include <vector>
#include <iostream>
#include "UserInterface.h"

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

/*
/////////////////////////////////////////
*/

class Task{
	private:
		
	public:
		TaskManager* assigned_task_manager;
		virtual void Execute() = 0;
		Task();
		~Task();
		Task(TaskManager* new_assigned_task_manager);
};

class TaskRunConsoleUserInterface : public Task{
	public:
		TaskRunConsoleUserInterface(TaskManager* new_assigned_task_manager);
		~TaskRunConsoleUserInterface();
		void Execute();

};

class TaskInitiateConsoleUserInterface : public Task{
	public:
		TaskInitiateConsoleUserInterface(TaskManager* new_assigned_task_manager);
		~TaskInitiateConsoleUserInterface();
		void Execute();
};


class TaskManager{
	private:
		std::list<Task*> tasks;
		UserInterface* user_interface;
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
			//Requests from other parts of program
		void RequestToInitiateConsoleUserInterface();
		void RequestToRunConsoleUserInterface();
};



#endif