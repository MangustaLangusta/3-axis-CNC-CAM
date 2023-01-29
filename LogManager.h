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
		std::string GetString() { return message; }
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
		void PushError(ErrorType _error_type, LogMessage _log_message){
			log.push_back(LogMessage(ErrorTypeToString(_error_type) + _log_message.GetString()));			
		}
		void PushMessage(LogMessage _log_message){
			log.push_back(_log_message);
		}
		void PrintError(ErrorType _error_type, LogMessage _log_message){
			std::cout<<ErrorTypeToString(_error_type)<<_log_message.GetString()<<std::endl;
			PushError(_error_type, _log_message);			
		}
		void PrintMessage(LogMessage _log_message){
			std::cout<<_log_message.GetString()<<std::endl;
			PushMessage(_log_message);
		}

};


#endif