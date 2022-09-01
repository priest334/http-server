#include "api_logger.h"

#include "utils/logger.h"
#include "service.h"


ApiLogger::ApiLogger(shared_ptr<HttpService> service)
	: simple::HttpMiddleware("ApiLogger"), service_(service) {
}

int ApiLogger::Process(shared_ptr<simple::HttpRequest> request, shared_ptr<simple::HttpResponse> response) {
	simple::logger::Info() << request->Url() << std::endl << request->Body();
	return 0;
}

int ApiLogger::OnProcessDone(shared_ptr<simple::HttpRequest> request, shared_ptr<simple::HttpResponse> response) {
	return 0;
}





