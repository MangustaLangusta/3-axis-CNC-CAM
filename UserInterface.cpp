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

void ConsoleUserInterface::Message(std::string new_text, ErrorCode new_error_code){
	messages_on_hold.emplace_back(new_text, new_error_code);
}

void ConsoleUserInterface::ProcessMessages(){
	bool fatal_error_exist = false;
	for(auto &it : messages_on_hold){
		DisplayMessage(it);
		if( Errors::IsFatal(it.GetErrorCode()) )
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
	task_manager_assigned->RequestToCreateNewProject("Test Project");
	state = MainMenu;
	AskTaskManagerToGiveControlBack();
}

void ConsoleUserInterface::RunQuitState(){
	std::cout<<"Exit."<<std::endl;
}

void ConsoleUserInterface::RunFatalError(){
	std::cout<<"Program will be terminated due to fatal error"<<std::endl;
	task_manager_assigned->RequestEmergencyStop();
}

void ConsoleUserInterface::AskTaskManagerToGiveControlBack() { 
	task_manager_assigned->RequestToRunConsoleUserInterface(); 
}

void ConsoleUserInterface::DisplayMessage(const UserInterfaceMessage &message) const{
	std::string error_str = "";
	if(message.GetErrorCode() != NORMAL)
		error_str = Errors::ErrorCodeToString(message.GetErrorCode()) + " ";
	std::cout<<error_str<<message.GetText()<<std::endl;
}

UserInterfaceMessage::UserInterfaceMessage(std::string new_text, ErrorCode new_error_code){
	text = new_text;
	error_code = new_error_code;
}
UserInterfaceMessage::~UserInterfaceMessage(){}

std::string UserInterfaceMessage::GetText() const {
	return text; 
}

ErrorCode UserInterfaceMessage::GetErrorCode() const{
	return error_code;
}

