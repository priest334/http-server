#include "api_result.h"

#include "api_error.h"


ApiResult::ApiResult()
	: result_(std::make_shared<simple::JsonDocument>()) {
	SetError(ApiError::OK);
}

ApiResult::ApiResult(shared_ptr<ApiError> api_error)
	: result_(std::make_shared<simple::JsonDocument>()) {
	SetError(api_error);
}

ApiResult& ApiResult::SetError(shared_ptr<ApiError> api_error) {
	result_->Set("errcode", api_error->ErrCode());
	result_->Set("errmsg", api_error->ErrMsg());
	return *this;
}

shared_ptr<simple::JsonDocument> ApiResult::operator->() {
	return result_;
}

shared_ptr<simple::JsonDocument> ApiResult::operator->() const {
	return result_;
}

shared_ptr<simple::JsonDocument> ApiResult::operator()() {
	return result_;
}

shared_ptr<simple::JsonDocument> ApiResult::operator()() const {
	return result_;
}

