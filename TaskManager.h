#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

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

/*
/////////////////////////////////////////
*/

class Task{
	private:
		
	public:
		TaskManager* assigned_task_manager;
		virtual void Execute() = 0;
		Task();
		Task(TaskManager* new_assigned_task_manager);
};

class TaskRunConsoleUI : public Task{
	public:
		TaskRunConsoleUI(TaskManager* new_assigned_task_manager);
		void Execute();

};

class TaskInitiateConsoleUI : public Task{
	public:
		TaskInitiateConsoleUI(TaskManager* new_assigned_task_manager);
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
		void StartTasksExecution();
		void ProhibitTasksExecution();
		void PermitTasksExecution();
		void AssignUserInterface(UserInterface* new_user_interface);
		UserInterface* GetUserInterface() const;
			//Requests from other parts of program
		void RequestToInitiateConsoleUI();
		void RequestToRunConsoleUI();
};


Task::Task(TaskManager* new_assigned_task_manager){
	assigned_task_manager = new_assigned_task_manager;	
}

TaskRunConsoleUI::TaskRunConsoleUI(TaskManager* new_assigned_task_manager) : Task(new_assigned_task_manager){}

TaskInitiateConsoleUI::TaskInitiateConsoleUI(TaskManager* new_assigned_task_manager) : Task(new_assigned_task_manager){}

void TaskManager::CommandLineArgumentsToTasks(int argc, char *argv[]){
	for(int i = 1; i < argc; i++){
		std::cout<<"argument "<<i<<" is: "<<argv[i]<<std::endl;
	}
	if(argc == 1){
		std::cout<<"No command line arguments found."<<std::endl;
		std::cout<<"Console UI will be started..."<<std::endl;
		RequestToRunConsoleUI();
	}
}

TaskManager::TaskManager(int argc, char *argv[]){
	user_interface = NULL;
	ProhibitTasksExecution();
	CommandLineArgumentsToTasks(argc, argv);	
}

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

void TaskManager::RequestToRunConsoleUI(){ 
	tasks.emplace_back(new TaskRunConsoleUI(this)); 
}

void TaskManager::RequestToInitiateConsoleUI(){
	tasks.emplace_back(new TaskInitiateConsoleUI(this));
}

void TaskRunConsoleUI::Execute(){
	
}

void TaskInitiateConsoleUI::Execute(){
	ConsoleUserInterface* new_user_interface = NULL;
	new_user_interface = new ConsoleUserInterface(assigned_task_manager);
	assigned_task_manager->AssignUserInterface(new_user_interface);
}


#endif