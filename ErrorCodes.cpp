#include "ErrorCodes.h"

const std::map<ErrorCode, std::string> Errors::error_codes_string_assignments = {
	{0x0000, "NORMAL"},
	{0x0001, "GENERAL_WARNING"},
	{0x5000, "GENERAL_ERROR"},
	{0xA000, "GENERAL_FATAL_ERROR"}
};

std::string Errors::ErrorCodeToString(ErrorCode error_code){
	std::string result = "";
	auto it = error_codes_string_assignments.find(error_code);
	if(it != error_codes_string_assignments.end())
		result = it->second;
	return result;
}

bool Errors::IsFatal(ErrorCode error_code){
	return (error_code >= GENERAL_FATAL_ERROR);
}