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
#include "Facets.h"
#include "Utility.h"
#include "BasicStructs.h"
#include "Contours.h"


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
class ContoursAggregator;

/*
/////////////////////////////////////////
*/
using Filename = std::string;


enum RequestCode {
	REQUEST_NIL,
	REQUEST_EMERGENCY_STOP,
	REQUEST_CREATE_NEW_PROJECT,
	REQUEST_MAKE_GCODE_FROM_FILE,
	REQUEST_PROCESS_INPUT_FILE,
	REQUEST_SPLIT_FACET_BODY_TO_CONTOURS,
	REQUEST_PATH_PATTERNS_FROM_CONTOURS
};

class RequestData{
	private:
		RequestCode request_code;
		Filename filename_data;
		ProjectSettings project_settings;
		SplitSettings split_settings;
		PathSettings path_settings;
	public:
		RequestData();
		~RequestData();
		void AddRequestCode(const RequestCode new_request_code);
		void AddFilename(const Filename new_filename);
		void AddProjectSettings(const ProjectSettings &new_project_settings);
		void AddSplitSettings(const SplitSettings new_split_settings);
		void AddPathSettings(const PathSettings &new_path_settings);
		bool IsValid();
		RequestCode GetRequestCode() const;
		Filename GetFilename() const;
		ProjectSettings GetProjectSettings() const;
		SplitSettings GetSplitSettings() const;
		PathSettings GetPathSettings() const;
};

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
		ProjectSettings project_settings;
	public:
		TaskCreateNewProject(TaskManager* new_assigned_task_manager, const ProjectSettings &new_project_settings);
		~TaskCreateNewProject();
		void Execute();	
};

class TaskUpdateProject : public Task{
	private:
		ProjectSettings project_settings;
	public:
		TaskUpdateProject(TaskManager* new_assigned_task_manager, const ProjectSettings &new_project_settings);
		~TaskUpdateProject();
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

class TaskSplitCompositeFacetBodyToContours : public Task{
	private:
		SplitSettings split_settings;
	public:
		TaskSplitCompositeFacetBodyToContours(TaskManager* new_assigned_task_manager, const SplitSettings new_split_settings);
		~TaskSplitCompositeFacetBodyToContours();
		void Execute();
};

class TaskPathPatternsFromContours : public Task{
	private:
		PathSettings path_settings;
	public:
		TaskPathPatternsFromContours(TaskManager* new_assigned_task_manager, const PathSettings &new_paths_settings);
		~TaskPathPatternsFromContours();
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
		void Request(const RequestData request_data);				//Requests from other parts of program to perform task specified in request_data			
};




#endif