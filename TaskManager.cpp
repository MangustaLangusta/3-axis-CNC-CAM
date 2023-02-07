#include "TaskManager.h"


Task::Task(TaskManager* new_assigned_task_manager){
	assigned_task_manager = new_assigned_task_manager;	
}

Task::~Task(){}

TaskRunConsoleUserInterface::TaskRunConsoleUserInterface(TaskManager* new_assigned_task_manager) : Task(new_assigned_task_manager){}

TaskRunConsoleUserInterface::~TaskRunConsoleUserInterface(){}

TaskInitiateConsoleUserInterface::TaskInitiateConsoleUserInterface(TaskManager* new_assigned_task_manager) : Task(new_assigned_task_manager){}

TaskInitiateConsoleUserInterface::~TaskInitiateConsoleUserInterface(){}

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

TaskManager::~TaskManager(){}

UserInterface* TaskManager::GetUserInterface() const{ return user_interface; }

void TaskManager::ProhibitTasksExecution(){ execution_permitted = false; }

void TaskManager::PermitTasksExecution(){ execution_permitted = true; }

bool TaskManager::IsExecutionPermitted() const { return execution_permitted; } 

void TaskManager::ExecuteNextTask() {
	if(tasks.empty())
		return;
	Task* task = *tasks.begin();
	if(task != NULL){
		std::cout<<"Execute next task..."<<std::endl;
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

void TaskManager::RequestToRunConsoleUserInterface(){ 
	tasks.emplace_back(new TaskRunConsoleUserInterface(this)); 
}

void TaskManager::RequestToInitiateConsoleUserInterface(){
	tasks.emplace_back(new TaskInitiateConsoleUserInterface(this));
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
