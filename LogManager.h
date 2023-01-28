#ifndef LOG_MANAGER
#define LOG_MANAGER

enum ErrorType { FatalError, Error, Warning };

class LogMessage{
	private:
		std::string message;
	public:
		LogMessage(std::string _message){
			message = _message;
		}
};

class LogManager{
	private:
		std::list<LogMessage> log;
		std::string ErrorTypeToString(ErrorType _error_type){
			switch(_error_type){
				case FatalError:
					return "FatalError! ";
				case Error:
					return "Error! ";
				case Warning:
					return "Warning! ";
			}
		}
	public:
		void PrintError(ErrorType _error_type, LogMessage _log_message){
			std::cout<<
			
			
		}
		void PrintMessage(LogMessage _log_message){
			
		}
		void PushError(ErrorType _error_type, LogMessage _log_message){
			
			
		}
		void PushMessage(LogMessage _log_message){
			
		}
};


#endif