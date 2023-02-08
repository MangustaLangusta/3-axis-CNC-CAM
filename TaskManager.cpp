#include "TaskManager.h"


Task::Task(TaskManager* new_assigned_task_manager){
	assigned_task_manager = new_assigned_task_manager;	
}

Task::~Task(){}

TaskRunConsoleUserInterface::TaskRunConsoleUserInterface(TaskManager* new_assigned_task_manager) : Task(new_assigned_task_manager){}

TaskRunConsoleUserInterface::~TaskRunConsoleUserInterface(){}

TaskInitiateConsoleUserInterface::TaskInitiateConsoleUserInterface(TaskManager* new_assigned_task_manager) : Task(new_assigned_task_manager){}

TaskInitiateConsoleUserInterface::~TaskInitiateConsoleUserInterface(){}

TaskCreateNewProject::TaskCreateNewProject(TaskManager* new_assigned_task_manager, std::string new_project_name) : Task(new_assigned_task_manager){
	project_name = new_project_name;
}

TaskCreateNewProject::~TaskCreateNewProject(){}

TaskEmergencyStop::TaskEmergencyStop(TaskManager* new_assigned_task_manager) : Task(new_assigned_task_manager){}

TaskEmergencyStop::~TaskEmergencyStop(){}

void TaskManager::CommandLineArgumentsToTasks(int argc, char *argv[]){
	for(int i = 1; i < argc; i++){
		std::cout<<"argument "<<i<<" is: "<<argv[i]<<std::endl;
	}
	if(argc == 1){
		std::cout<<"No command line arguments found."<<std::endl;
		std::cout<<"Console UI will be started..."<<std::endl;
		RequestToInitiateConsoleUserInterface();
	}
}

TaskManager::TaskManager(int argc, char *argv[]){
	user_interface = NULL;
	ProhibitTasksExecution();
	CommandLineArgumentsToTasks(argc, argv);	
}

TaskManager::~TaskManager(){
	for(auto &it : tasks)
		delete it;
	delete user_interface;
	delete assigned_project;
}

UserInterface* TaskManager::GetUserInterface() const{ return user_interface; }

void TaskManager::ProhibitTasksExecution(){ execution_permitted = false; }

void TaskManager::PermitTasksExecution(){ execution_permitted = true; }

bool TaskManager::IsExecutionPermitted() const { return execution_permitted; } 

void TaskManager::ExecuteNextTask() {
	if(tasks.empty())
		return;
	Task* task = *tasks.begin();
	if(task != NULL){
		task->Execute();
		delete task;
	}
	tasks.pop_front();
}

bool TaskManager::HaveTasksToDo() const { return (!tasks.empty()); }

void TaskManager::StartTasksExecution(){
	PermitTasksExecution();
	while(IsExecutionPermitted() && HaveTasksToDo()){
		ExecuteNextTask();
	}
}

void TaskManager::AssignUserInterface(UserInterface* new_user_interface){
	if(user_interface != NULL)
		delete user_interface;
	user_interface = new_user_interface;	
}

void TaskManager::AssignProject(Project* new_project){
	assigned_project = new_project;
}

void TaskManager::MessageToUserInterface(std::string text){
	user_interface->Message(text, NORMAL);
}

void TaskManager::ErrorMessageToUserInterface(std::string text, ErrorCode error_code){
	user_interface->Message(text, error_code);
}

void TaskManager::RequestEmergencyStop(){
	TaskEmergencyStop* new_task = new TaskEmergencyStop(this);
	auto it = tasks.begin();
	if(!tasks.empty())
		it++;
	tasks.emplace(it, new_task);
}

void TaskManager::RequestToRunConsoleUserInterface(){ 
	tasks.emplace_back(new TaskRunConsoleUserInterface(this)); 
}

void TaskManager::RequestToInitiateConsoleUserInterface(){
	tasks.emplace_back(new TaskInitiateConsoleUserInterface(this));
}

void TaskManager::RequestToCreateNewProject(std::string project_name){
	tasks.emplace_back(new TaskCreateNewProject(this, project_name));
}

void TaskRunConsoleUserInterface::Execute(){
	assigned_task_manager->GetUserInterface()->Run();
}

void TaskInitiateConsoleUserInterface::Execute(){
	ConsoleUserInterface* new_user_interface = NULL;
	new_user_interface = new ConsoleUserInterface(assigned_task_manager);
	assigned_task_manager->AssignUserInterface(new_user_interface);
	assigned_task_manager->RequestToRunConsoleUserInterface();
}

void TaskCreateNewProject::Execute(){
	Project* new_project = NULL;
	new_project = new Project(project_name);
	assigned_task_manager->AssignProject(new_project);
	assigned_task_manager->MessageToUserInterface("New project assigned to TaskManager");
}

void TaskEmergencyStop::Execute(){
	assigned_task_manager->ProhibitTasksExecution();	
}