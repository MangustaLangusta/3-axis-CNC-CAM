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
	finish_session = true;
	do{
		RunCurrentState();
	} while(!finish_session);
}

void ConsoleUserInterface::RunCurrentState(){
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
	return;
}

void ConsoleUserInterface::RunMainMenuState(){
	return;
}

void ConsoleUserInterface::RunQuickStartState(){
	return;
}

void ConsoleUserInterface::RunQuitState(){
	return;
}


void ConsoleUserInterface::AskTaskManagerToGiveControlBack() { task_manager_assigned->RequestToRunConsoleUserInterface(); }

