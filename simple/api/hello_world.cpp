#include "hello_world.h"

#include "utils/json_document.h"
#include "utils/string_helper.h"
#include "api_error.h"
#include "api_result.h"
#include "service.h"


HelloWorld::HelloWorld(shared_ptr<HttpService> service)
	: ApiGetHandler(service) {
}

string HelloWorld::Path() const {
	return "/helloworld";
}

int HelloWorld::Process(shared_ptr<simple::HttpRequest> request, shared_ptr<simple::HttpResponse> response) {
	response->SetPayload("hello world");
	return 0;
}
