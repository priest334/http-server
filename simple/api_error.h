#pragma once
#ifndef simple_api_api_error_h_
#define simple_api_api_error_h_

#include <memory>
#include <string>


using std::shared_ptr;
using std::string;
class ApiError {
public:
	ApiError(int errcode, const string& errmsg);
	int ErrCode() const;
	string ErrMsg() const;

	static shared_ptr<ApiError> OK;
	static shared_ptr<ApiError> InternalServerError;
private:
	int errcode_;
	string errmsg_;
};

#endif // simple_api_api_error_h_

