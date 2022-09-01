#pragma once
#ifndef simple_api_api_handler_h_
#define simple_api_api_handler_h_

#include <string>
#include "server/http/route.h"
#include "server/http/request.h"
#include "server/http/response.h"


using std::string;
using std::weak_ptr;

typedef enum api_method : int {
	GET,
	POST
} ApiMethod;

const char* const api_method_list[] = {
	"GET",
	"POST"
};

class HttpService;
template<ApiMethod _method>
class ApiHandler
	: public simple::HttpRoute {
	weak_ptr<HttpService> service_;
public:
	ApiHandler(shared_ptr<HttpService> service)
		: service_(service) {
	}

	shared_ptr<HttpService> GetService() const {
		return service_.lock();
	}

	bool MatchVerb(const string& verb) const override {
		return api_method_list[_method] == verb;
	}

	virtual ~ApiHandler() {
	}
};


typedef ApiHandler<ApiMethod::GET> ApiGetHandler;
typedef ApiHandler<ApiMethod::POST> ApiPostHandler;

#endif // simple_api_api_handler_h_

