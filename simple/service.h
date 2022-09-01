#pragma once
#ifndef SIMPLE_HTTP_SERVICE_H_
#define SIMPLE_HTTP_SERVICE_H_

#include <unordered_map>
#include "utils/config.h"
#include "server/http/application.h"
#include "server/http_server.h"


using std::unordered_map;
class HttpService
	: public simple::HttpApplication,
	public std::enable_shared_from_this<HttpService> {
public:
	HttpService(shared_ptr<simple::Config> config);
	string Name() const override;
	int Start(const string& ip, int port);
private:
	shared_ptr<simple::HttpServer> server_;
	shared_ptr<simple::Config> config_;
};

#endif // SIMPLE_HTTP_SERVICE_H_

