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
	do{
		RunCurrentState();
	} while(!finish_session);
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
	std::cout<<"Some actions"<<std::endl;
	state = MainMenu;
	AskTaskManagerToGiveControlBack();
}

void ConsoleUserInterface::RunQuitState(){
	std::cout<<"Exit."<<std::endl;
}


void ConsoleUserInterface::AskTaskManagerToGiveControlBack() { task_manager_assigned->RequestToRunConsoleUserInterface(); }

