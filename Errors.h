#ifndef ERROR_CODES_H
#define ERROR_CODES_H

#include <map>
#include <string>
#include <list>

/*			NO ERRORS												*/
#define NORMAL															0x0000
/****************************************/
/*			WARNINGS												*/
#define GENERAL_WARNING		 									0x0001

/****************************************/
/*			ERRORS													*/
#define GENERAL_ERROR																	0x5000
#define ERROR_STL_FILE_INVALID												0x5001
#define ERROR_STL_FILE_UNEXPECTED_EOF 								0x5002
#define ERROR_STL_FILE_EOF_NOT_REACHED								0x5003
#define ERROR_FACET_BODY_UNABLE_TO_CREATE							0x5004
#define ERROR_FACET_BODY_VALIDITY_CHECK_NOT_PASSED		0x5005
#define ERROR_UNABLE_TO_SPLIT_FACET_BODY_TO_CONTOURS	0x5006
/****************************************/
/*			FATAL ERRORS										*/
#define GENERAL_FATAL_ERROR									0xA000

/****************************************/

using ErrorCode = long;

namespace Errors {
	extern const std::map<ErrorCode, std::string> error_codes_string_assignments;
	std::string ErrorCodeToString(ErrorCode error_code);
	bool IsFatal(ErrorCode error_code);
};

class Error{
	public:
		ErrorCode code;
		Error();
		Error(ErrorCode new_code);
		~Error();
		bool IsFatal();
};

class ErrorsLog{
	private:
		std::list<Error> errors;
	public:
		ErrorsLog();
		~ErrorsLog();
		bool HaveErrors();
		std::list<Error> GetErrors() const;
		void Clear();
};

#endif