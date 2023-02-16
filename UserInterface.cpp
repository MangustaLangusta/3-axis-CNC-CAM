#include "UserInterface.h"


UserInterface::UserInterface(TaskManager* new_task_manager_assigned){
	task_manager_assigned = new_task_manager_assigned;
}

UserInterface::~UserInterface(){}

ConsoleUserInterface::ConsoleUserInterface(TaskManager* new_task_manager_assigned) : UserInterface(new_task_manager_assigned){
	state = Initial;
}

ConsoleUserInterface::~ConsoleUserInterface(){}

void ConsoleUserInterface::Run(){
	ProcessMessages();
	do{
		RunCurrentState();
	} while(!finish_session);
}

void ConsoleUserInterface::Message(std::string new_text, const Error &new_error){
	UserInterfaceMessage new_message(new_text, new_error);
	DisplayMessage(new_message);
	messages_on_hold.emplace_back(new_text, new_error);
}

void ConsoleUserInterface::ProcessMessages(){
	bool fatal_error_exist = false;
	for(auto &it : messages_on_hold){
		if( it.GetError().IsFatal() )
			fatal_error_exist = true;
	}	
	if(fatal_error_exist)
		state = FatalError;
}

void ConsoleUserInterface::RunCurrentState(){
	//system("cls");
	finish_session = true;
	switch(state){
		case Initial:
			RunInitialState();
			break;
		case MainMenu:
			RunMainMenuState();
			break;
		case QuickStart:
			RunQuickStartState();
			break;
		case Quit:
			RunQuitState();
			break;
		case FatalError:
			RunFatalError();
			break;
		default:
			RunQuitState();
	}
}

void ConsoleUserInterface::ChangeState(ConsoleUserInterfaceState new_state){
	state = new_state;
	finish_session = false;
}

void ConsoleUserInterface::RunInitialState(){
	//system("cls");
	std::cout<<"3-Axis CNC CAM tool."<<std::endl;
	std::cout<<"This is beta-version"<<std::endl;
	std::cout<<std::endl;
	ChangeState(MainMenu);
}

void ConsoleUserInterface::RunMainMenuState(){
	//system("cls");
	std::vector<ConsoleUserInterfaceState> variants_to_go;
	std::map<int, std::string> menu_assignments;
	ConsoleUserInterfaceState state_to_go;
	int input_option;
	
	variants_to_go = states_travel_map.find(state)->second;
	
	std::cout<<std::endl;
	std::cout<<"MainMenu."<<std::endl;
	std::cout<<"Choose action by typing number of option:"<<std::endl<<std::endl;
	for(int i = 1; i <= variants_to_go.size(); i++){
		menu_assignments.insert( std::make_pair(i, states_string_assignments.find(variants_to_go[i-1])->second) );
		std::cout<<i<<". "<<menu_assignments.find(i)->second<<std::endl;
	}
	std::cout<<std::endl;
	std::cin>>input_option;
	std::cout<<std::endl;
	
	while( (input_option < 1) || (input_option > variants_to_go.size()) ){
		std::cout<<"Please, type number corrdesponding to desired option"<<std::endl;
		std::cin>>input_option;
		std::cout<<std::endl;
	}
		ChangeState(variants_to_go[input_option-1]);
}

void ConsoleUserInterface::RunQuickStartState(){
	std::cout<<"QuickStart"<<std::endl;
	RequestData project_request, process_file_request;
	project_request.AddRequestCode(REQUEST_CREATE_NEW_PROJECT);
	project_request.AddProjectName(ProjectName("TestProject"));
	assert(project_request.IsValid());
	task_manager_assigned->Request(project_request);
	process_file_request.AddFilename(Filename("test.STL"));
	process_file_request.AddRequestCode(REQUEST_PROCESS_INPUT_FILE);
	assert(process_file_request.IsValid());
	task_manager_assigned->Request(process_file_request);
	state = MainMenu;
}

void ConsoleUserInterface::RunQuitState(){
	std::cout<<"Exit."<<std::endl;
	task_manager_assigned->ProhibitTasksExecution();
}

void ConsoleUserInterface::RunFatalError(){
	std::cout<<"Program will be terminated due to fatal error"<<std::endl;
	RequestData request_emergency_stop;
	request_emergency_stop.AddRequestCode(REQUEST_EMERGENCY_STOP);
	assert(request_emergency_stop.IsValid());
	task_manager_assigned->Request(request_emergency_stop);
}

void ConsoleUserInterface::DisplayMessage(const UserInterfaceMessage &message) const{
	std::string error_str = "";
	if(message.GetErrorCode() != NORMAL)
		error_str = Errors::ErrorCodeToString(message.GetErrorCode()) + " ";
	std::cout<<error_str<<message.GetText()<<std::endl;
}

UserInterfaceMessage::UserInterfaceMessage(std::string new_text, const Error &new_error){
	text = new_text;
	error = new_error;
}
UserInterfaceMessage::~UserInterfaceMessage(){}

std::string UserInterfaceMessage::GetText() const {
	return text; 
}

Error UserInterfaceMessage::GetError() const{
	return error;
}

ErrorCode UserInterfaceMessage::GetErrorCode() const{
	return error.code;
}

