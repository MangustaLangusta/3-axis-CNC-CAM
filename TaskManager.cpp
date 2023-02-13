#include "TaskManager.h"


Task::Task(TaskManager* new_assigned_task_manager){
	assigned_task_manager = new_assigned_task_manager;	
}

Task::~Task(){}

void Task::Message(const std::string text){
	assigned_task_manager->MessageToUserInterface(text);
}

void Task::ErrorMessage(const std::string text, const Error error){
	assigned_task_manager->ErrorMessageToUserInterface(text, error);
}

TaskCreateNewProject::TaskCreateNewProject(TaskManager* new_assigned_task_manager, std::string new_project_name) : Task(new_assigned_task_manager){
	project_name = new_project_name;
}

TaskCreateNewProject::~TaskCreateNewProject(){}

TaskEmergencyStop::TaskEmergencyStop(TaskManager* new_assigned_task_manager) : Task(new_assigned_task_manager){}

TaskEmergencyStop::~TaskEmergencyStop(){}

TaskProcessInputFile::TaskProcessInputFile(TaskManager* new_assigned_task_manager, std::string new_filename) : Task(new_assigned_task_manager){
	filename = new_filename;
}

TaskProcessInputFile::~TaskProcessInputFile(){}

void TaskManager::CommandLineArgumentsToTasks(int argc, char *argv[]){
	for(int i = 1; i < argc; i++){
		std::cout<<"argument "<<i<<" is: "<<argv[i]<<std::endl;
	}
	if(argc == 1){
		std::cout<<"No command line arguments found."<<std::endl;
		std::cout<<"Console UI will be started..."<<std::endl;
		
		StartConsoleUserInterface();
	}
}

TaskManager::TaskManager(int argc, char *argv[]){
	user_interface = NULL;
	assigned_project = NULL;
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

void TaskManager::ProhibitTasksExecution(){ 
	execution_permitted = false; 
}

void TaskManager::PermitTasksExecution(){ execution_permitted = true; }

bool TaskManager::IsExecutionPermitted() const { 
	return execution_permitted; 
} 

void TaskManager::StartConsoleUserInterface(){
	if(user_interface != NULL)
		delete user_interface;
	ConsoleUserInterface* new_console_user_interface = new ConsoleUserInterface(this);
	user_interface = new_console_user_interface;
}

void TaskManager::ExecuteNextTask() {
	if(tasks.empty())
		user_interface->Run();
	Task* task = *tasks.begin();
	if(task != NULL){
		task->Execute();
		delete task;
	}
	tasks.pop_front();
}

void TaskManager::StartTasksExecution(){
	PermitTasksExecution();
	while(IsExecutionPermitted()){
		ExecuteNextTask();
	}
}

void TaskManager::AssignProject(Project* new_project){
	assigned_project = new_project;
}

Project* TaskManager::GetAssignedProject(){
	return assigned_project;
}

void TaskManager::MessageToUserInterface(const std::string text){
	user_interface->Message(text, Error(NORMAL));
}

void TaskManager::ErrorMessageToUserInterface(const std::string text, const Error error){
	user_interface->Message(text, error);
}

void TaskManager::Request(RequestCode request_code){
	
	return;
}	

void TaskManager::Request(RequestCode request_code, std::string str){
	return;
	
}

void TaskManager::RequestEmergencyStop(){
	TaskEmergencyStop* new_task = new TaskEmergencyStop(this);
	auto it = tasks.begin();
	if(!tasks.empty())
		it++;
	tasks.emplace(it, new_task);
}

void TaskManager::RequestToCreateNewProject(std::string project_name){
	tasks.emplace_back(new TaskCreateNewProject(this, project_name));
}

void TaskManager::RequestToProcessInputFile(std::string filename){
	tasks.emplace_back(new TaskProcessInputFile(this, filename));
}

void TaskCreateNewProject::Execute(){
	Project* new_project = NULL;
	new_project = new Project(project_name);
	assigned_task_manager->AssignProject(new_project);
	Message("New project assigned to TaskManager");
}

void TaskEmergencyStop::Execute(){
	assigned_task_manager->ProhibitTasksExecution();	
}

void TaskProcessInputFile::Execute(){
	std::list<Error> errors_list;
	std::list<ErrorCode> error_codes_list;	//to remove
	Project* project = NULL;
	RawSTL* tmp_stl = NULL;
	CompositeFacetBody* composite_facet_body = NULL;
	
	project = assigned_task_manager->GetAssignedProject();
	assert(project != NULL);
	project->SetFileName(filename);
	Message("Start processing file '"+filename+"'");
	tmp_stl = new RawSTL(filename, &error_codes_list);	//to change to list<Error>
	if(!tmp_stl->IsValid()){
		ErrorMessage("", Error(ERROR_STL_FILE_INVALID));
		return;
	}
	Message("STL file successfully processed");
	composite_facet_body = new CompositeFacetBody(tmp_stl->GetTriangles3D(), &errors_list);
	delete tmp_stl;
	project->AssignCompositeFacetBody(composite_facet_body);
	composite_facet_body->GetZExtremums();
}