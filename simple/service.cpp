#include "service.h"

#include "api_logger.h"
#include "api/hello_world.h"


HttpService::HttpService(shared_ptr<simple::Config> config)
	: config_(config) {
}

string HttpService::Name() const {
	string name = config_->Get("server.name");
	if (name.empty()) {
		return HttpApplication::Name();
	}
	return name;
}

int HttpService::Start(const string& ip, int port) {
	server_ = std::make_shared<simple::HttpServer>(shared_from_this());
	InstallMiddleware(std::make_shared<ApiLogger>(shared_from_this()));
	InstallRoute(std::make_shared<HelloWorld>(shared_from_this()));
	return server_->Start(ip, port);
}
