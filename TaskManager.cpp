#include "TaskManager.h"

RequestData::RequestData(){
	request_code = REQUEST_NIL;
}

RequestData::~RequestData(){}

void RequestData::AddRequestCode(const RequestCode new_request_code){
	request_code = new_request_code;
}

void RequestData::AddFilename(const Filename new_filename){
	filename_data = new_filename;
}

void RequestData::AddProjectName(const ProjectName new_project_name){
	project_name_data = new_project_name;
}

void RequestData::AddSplitSettings(const SplitSettings new_split_settings){
	split_settings = new_split_settings;
}

bool RequestData::IsValid(){
	switch(request_code){
		//Cases where no additional info required:
		case REQUEST_EMERGENCY_STOP:
			return true;
		//Cases wher only project name required:
		case REQUEST_CREATE_NEW_PROJECT:
			if(project_name_data != "")
				return true;
			return false;
		//Cases where filename required:
		case REQUEST_MAKE_GCODE_FROM_FILE:
		case REQUEST_PROCESS_INPUT_FILE:
			if(filename_data != "")
				return true;
			return false;
		//Case where correct split settings required
		case REQUEST_SPLIT_FACET_BODY_TO_CONTOURS:
			return split_settings.IsValid();
		//All wrong names (including NIL code):
		default:
			return false;
	}
}

RequestCode RequestData::GetRequestCode() const{
	return request_code;
}

Filename RequestData::GetFilename() const{
	return filename_data;
}

ProjectName RequestData::GetProjectName() const{
	return project_name_data;
}

SplitSettings RequestData::GetSplitSettings() const{
	return split_settings;
}

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

TaskSplitCompositeFacetBodyToContours::TaskSplitCompositeFacetBodyToContours(TaskManager* new_assigned_task_manager, 
																																						const SplitSettings new_split_settings) : Task(new_assigned_task_manager){
	split_settings= new_split_settings;
}
		
TaskSplitCompositeFacetBodyToContours::~TaskSplitCompositeFacetBodyToContours(){}

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
	if(tasks.empty())
		return;
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

void TaskManager::Request(const RequestData request_data){
	switch(request_data.GetRequestCode()){
		case REQUEST_EMERGENCY_STOP:
			break;
		case REQUEST_CREATE_NEW_PROJECT:
			tasks.emplace_back(new TaskCreateNewProject(this, request_data.GetProjectName()));
			break;
		case REQUEST_MAKE_GCODE_FROM_FILE:
			break;
		case REQUEST_PROCESS_INPUT_FILE:
			tasks.emplace_back(new TaskProcessInputFile(this, request_data.GetFilename()));
			break;
		case REQUEST_SPLIT_FACET_BODY_TO_CONTOURS:
			tasks.emplace_back( new TaskSplitCompositeFacetBodyToContours(this, request_data.GetSplitSettings()) );
			break;
		default:
			assert(false);
	}
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
	bool error_flag = false;
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
	composite_facet_body = new CompositeFacetBody(tmp_stl->GetTriangles3D(), &errors_list, &error_flag);
	delete tmp_stl;
	if(error_flag){
		for(auto &it : errors_list)
			ErrorMessage("", it);
		return;
	}
	Message("Composite Facet Body successfully created");
	project->AssignCompositeFacetBody(composite_facet_body);
}

void TaskSplitCompositeFacetBodyToContours::Execute(){
	std::list<Error> errors_list;
	ErrorFlag error_flag;
	std::vector<Plane3D> split_planes;
	std::pair<double, double> boundaries;
	std::vector<std::list<Point3D>> raw_contours;
	CompositeFacetBody* composite_body;
	Project *project;
	ContoursAggregator* contours_aggregator;
	
	project = assigned_task_manager->GetAssignedProject();
	assert(project != NULL);
	
	composite_body = project->GetAssignedCompositeFacetBody();
	assert(composite_body != NULL);
	
	if(split_settings.IsWholeZRange())
		boundaries = composite_body->GetZExtremums();
	else 
		boundaries = std::make_pair(split_settings.z_max, split_settings.z_min);
	split_planes = MathOperations::CreateZPlanesArray(boundaries, split_settings.spacing);
	
	raw_contours = composite_body->SplitByZPlanes(split_planes, &errors_list, &error_flag);
	std::cout<<"size = "<<raw_contours.size()<<std::endl;
		//handling errors during split to raw contours
	if(error_flag.HaveErrors()){
		for(auto &it : errors_list){
			ErrorMessage("", it);
		}
		Message("Split task cancelled");
		return;
	}
	else {
		Message("Facet body splitted by z contours");
	}
	
	//ContoursAggregator(const std::vector<std::list<Point3D>> source_contours, std::list<Error> *errors_list, ErrorFlag *error_flag)
	contours_aggregator = new ContoursAggregator(raw_contours, &errors_list, &error_flag);

		//handling errors during creation of contours aggregator
	if(error_flag.HaveErrors()){
		for(auto &it : errors_list){
			ErrorMessage("", it);
		}
		Message("Split task cancelled");
		delete contours_aggregator;
		return;
	}
	else {
		Message("Contours aggregator created");
	}
	
	project->AssignContoursAggregator(contours_aggregator);
	
	
	return;
}