#include "Errors.h"

const std::map<ErrorCode, std::string> Errors::error_codes_string_assignments = {
	{0x0000, "NORMAL"},
	{0x0001, "GENERAL_WARNING"},
	{0x0002, "WARNING_CONTOUR_NOT_IN_SINGLE_PLANE"},
	{0x5000, "GENERAL_ERROR"},
	{0x5001, "ERROR_STL_FILE_INVALID"},
	{0x5002, "ERROR_STL_FILE_UNEXPECTED_EOF"},
	{0x5003, "ERROR_STL_FILE_EOF_NOT_REACHED"},
	{0x5004, "ERROR_FACET_BODY_UNABLE_TO_CREATE"},
	{0x5005, "ERROR_FACET_BODY_VALIDITY_CHECK_NOT_PASSED"},
	{0x5006, "ERROR_UNABLE_TO_SPLIT_FACET_BODY_TO_CONTOURS"},
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

ErrorFlag::ErrorFlag(){
	warning_flag = false;
	error_flag = false;
	fatal_error_flag = false;
}

ErrorFlag::~ErrorFlag(){}

ErrorFlag::ClearFlags(){
	warning_flag = false;
	error_flag = false;
	fatal_error_flag = false;
}

Error::Error(){}

Error::Error(ErrorCode new_code){
	code = new_code;
}

Error::~Error(){}

bool Error::IsFatal(){
	return 	(code >= GENERAL_FATAL_ERROR);
}

ErrorsLog::ErrorsLog(){}

ErrorsLog::~ErrorsLog(){}

bool ErrorsLog::HaveErrors(){
	return !errors.empty();
}
std::list<Error> ErrorsLog::GetErrors() const{
	return errors;
}

void ErrorsLog::Clear(){
	errors.clear();
}
