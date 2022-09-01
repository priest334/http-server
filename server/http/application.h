#pragma once
#ifndef SERVER_HTTP_APPLICATION_H_
#define SERVER_HTTP_APPLICATION_H_

#include <memory>
#include <string>


using std::shared_ptr;
using std::string;
namespace simple {
	class HttpRoute;
	class HttpRoutes;
	class HttpMiddleware;
	class HttpRequest;
	class HttpResponse;

	class HttpApplication {
	public:
		HttpApplication();
		virtual ~HttpApplication();
		virtual string Name() const;
		void InstallRoute(shared_ptr<HttpRoute> route);
		void InstallMiddleware(shared_ptr<HttpMiddleware> middleware);
		int Process(shared_ptr<HttpRequest> request, shared_ptr<HttpResponse> response);
	protected:
		shared_ptr<HttpRoutes> routes_;
		shared_ptr<HttpMiddleware> middleware_;
	};
} // namespace simple

#endif // SERVER_HTTP_APPLICATION_H_

