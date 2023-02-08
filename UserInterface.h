#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include <map>
#include <string>
#include <list>
#include "TaskManager.h"
#include "Project.h"
#include "ErrorCodes.h"

class TaskManager;
class UserInterfaceMessage;

enum ConsoleUserInterfaceState {Initial, MainMenu, QuickStart, Quit, FatalError};

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
		std::list<UserInterfaceMessage> messages_on_hold;
		virtual void ProcessMessages() = 0;
	public:
		UserInterface(TaskManager* new_task_manager_assigned);
		~UserInterface();
		virtual void Run() = 0;	
		virtual void Message(std::string new_text, ErrorCode new_error_code) = 0;
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
		void RunFatalError();
		void AskTaskManagerToGiveControlBack();
		void ChangeState(ConsoleUserInterfaceState new_state);
		void ProcessMessages();
		void DisplayMessage(const UserInterfaceMessage &message) const;
	public:
		ConsoleUserInterface(TaskManager* new_task_manager_assigned);
		~ConsoleUserInterface();
		void Run();
		void Message(std::string new_text, ErrorCode new_error_code);
};

class UserInterfaceMessage{
	private:
		std::string text;
		ErrorCode error_code;
	public:
		UserInterfaceMessage(std::string new_text, ErrorCode new_error_code);
		~UserInterfaceMessage();
		std::string GetText() const;	
		ErrorCode GetErrorCode() const;
};

#endif