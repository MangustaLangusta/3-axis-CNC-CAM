#ifndef ERROR_CODES_H
#define ERROR_CODES_H

#include <map>
#include <string>

/*			NO ERRORS												*/
#define NORMAL								0x0000
/****************************************/
/*			WARNINGS												*/
#define GENERAL_WARNING		 		0x0001

/****************************************/
/*			ERRORS													*/
#define GENERAL_ERROR					0x5000

/****************************************/
/*			FATAL ERRORS										*/
#define GENERAL_FATAL_ERROR		0xA000

/****************************************/

using ErrorCode = long;

namespace Errors {
	extern const std::map<ErrorCode, std::string> error_codes_string_assignments;
	std::string ErrorCodeToString(ErrorCode error_code);
	bool IsFatal(ErrorCode error_code);
};

#endif