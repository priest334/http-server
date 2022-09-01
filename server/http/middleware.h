#pragma once
#ifndef SERVER_HTTP_MIDDLEWARE_H_
#define SERVER_HTTP_MIDDLEWARE_H_

#include <memory>
#include <string>


using std::shared_ptr;
using std::string;
namespace simple {
	class HttpRequest;
	class HttpResponse;
	class HttpRoute;

	class HttpMiddleware {
		friend class HttpApplication;
	public:
		HttpMiddleware(const string& name);
		virtual ~HttpMiddleware();
		void Append(shared_ptr<HttpMiddleware> middleware);
		virtual int Process(shared_ptr<HttpRequest> request, shared_ptr<HttpResponse> response) = 0;
		virtual int OnProcessDone(shared_ptr<HttpRequest> request, shared_ptr<HttpResponse> response);
	protected:
		int Process(shared_ptr<HttpRoute> route, shared_ptr<HttpRequest> request, shared_ptr<HttpResponse> response);
		string name_;
		shared_ptr<HttpMiddleware> sibling_;
	};
} // namespace simple

#endif // SERVER_HTTP_MIDDLEWARE_H_

