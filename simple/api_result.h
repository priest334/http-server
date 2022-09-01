#pragma once
#ifndef simple_api_api_result_h_
#define simple_api_api_result_h_

#include "utils/json_document.h"


class ApiError;
class ApiResult {
public:
	ApiResult();
	ApiResult(shared_ptr<ApiError> api_error);
	ApiResult& SetError(shared_ptr<ApiError> api_error);
	shared_ptr<simple::JsonDocument> operator->();
	shared_ptr<simple::JsonDocument> operator->() const;
	shared_ptr<simple::JsonDocument> operator()();
	shared_ptr<simple::JsonDocument> operator()() const;
protected:
	shared_ptr<simple::JsonDocument> result_;
};

#endif // simple_api_api_result_h_

