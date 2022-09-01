#include "api_error.h"


ApiError::ApiError(int errcode, const string& errmsg)
	: errcode_(errcode), errmsg_(errmsg.c_str()) {
}

int ApiError::ErrCode() const {
	return errcode_;
}

string ApiError::ErrMsg() const {
	return errmsg_;
}

shared_ptr<ApiError> ApiError::OK = std::make_shared<ApiError>(0, "OK");
shared_ptr<ApiError> ApiError::InternalServerError = std::make_shared<ApiError>(500, "Internal Server Error");
