#pragma once
#ifndef simple_api_hello_world_h_
#define simple_api_hello_world_h_

#include "api_handler.h"


class HelloWorld
	: public ApiGetHandler {
public:
	HelloWorld(shared_ptr<HttpService> service);
	string Path() const override;
	int Process(shared_ptr<simple::HttpRequest> request, shared_ptr<simple::HttpResponse> response) override;
};

#endif // simple_api_hello_world_h_

