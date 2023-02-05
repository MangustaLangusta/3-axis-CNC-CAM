#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

class TaskManager;

class UserInterface{
	private:
		TaskManager *task_manager_assigned;
	public:
		UserInterface(TaskManager* new_task_manager_assigned);
		virtual void Run() = 0;
		virtual void GetHelp() = 0;
	
};

class ConsoleUserInterface : public UserInterface{
	private:
		void GetHelp();
	public:
		ConsoleUserInterface(TaskManager* new_task_manager_assigned);
		void Run();
		void Stop();
};

UserInterface::UserInterface(TaskManager* new_task_manager_assigned){
	task_manager_assigned = new_task_manager_assigned;
}

ConsoleUserInterface::ConsoleUserInterface(TaskManager* new_task_manager_assigned) : UserInterface(new_task_manager_assigned){
	std::cout<<"ConsoleUI construction done"<<std::endl;
}

void ConsoleUserInterface::Run(){
	std::cout<<"Run console user interface"<<std::endl;	
}

void ConsoleUserInterface::GetHelp(){
	std::cout<<"Get help for console UI called"<<std::endl;
}





#endif