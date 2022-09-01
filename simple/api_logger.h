#pragma once
#ifndef simple_api_api_logger_h_
#define simple_api_api_logger_h_

#include <string>
#include "server/http/middleware.h"
#include "server/http/request.h"
#include "server/http/response.h"


using std::string;
using std::weak_ptr;
class HttpService;
class ApiLogger
	: public simple::HttpMiddleware {
public:
	ApiLogger(shared_ptr<HttpService> service);
	int Process(shared_ptr<simple::HttpRequest> request, shared_ptr<simple::HttpResponse> response) override;
	int OnProcessDone(shared_ptr<simple::HttpRequest> request, shared_ptr<simple::HttpResponse> response) override;
private:
	weak_ptr<HttpService> service_;
};

#endif // simple_api_api_logger_h_

