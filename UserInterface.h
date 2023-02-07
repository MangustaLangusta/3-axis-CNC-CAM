#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include <map>
#include "TaskManager.h"

class TaskManager;

enum ConsoleUserInterfaceState {Initial, MainMenu, QuickStart, Quit};

const std::map<ConsoleUserInterfaceState, std::vector<ConsoleUserInterfaceState>> states_travel_map = {
	{ MainMenu, {QuickStart, Quit} },
	{ QuickStart, {MainMenu, Quit} }	
};

const std::map<ConsoleUserInterfaceState, std::string> states_string_assignments = {
	{	MainMenu, "MainMenu"},
	{	QuickStart, "QuickStart"},
	{	Quit, "Exit"}
};

class UserInterface{
	protected:
		TaskManager *task_manager_assigned;
	public:
		UserInterface(TaskManager* new_task_manager_assigned);
		~UserInterface();
		virtual void Run() = 0;	
};

class ConsoleUserInterface : public UserInterface{
	private:
		ConsoleUserInterfaceState state;
		bool finish_session;
		void RunCurrentState();
		void RunInitialState();
		void RunMainMenuState();
		void RunQuickStartState();
		void RunQuitState();
		void AskTaskManagerToGiveControlBack();
		void ChangeState(ConsoleUserInterfaceState new_state);
	public:
		ConsoleUserInterface(TaskManager* new_task_manager_assigned);
		~ConsoleUserInterface();
		void Run();
};




#endif