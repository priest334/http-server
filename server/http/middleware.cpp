#include "middleware.h"

#include "route.h"
#include "utils/logger.h"
#include "request.h"
#include "response.h"


namespace simple {
	HttpMiddleware::HttpMiddleware(const string& name)
		: name_(name) {
	}

	HttpMiddleware::~HttpMiddleware() {
	}

	void HttpMiddleware::Append(shared_ptr<HttpMiddleware> middleware) {
		if (sibling_) {
			sibling_->Append(middleware);
		} else {
			sibling_ = middleware;
		}
	}

	int HttpMiddleware::Process(shared_ptr<HttpRoute> route, shared_ptr<HttpRequest> request, shared_ptr<HttpResponse> response) {
		int code = Process(request, response);
		if (code < 0) {
			logger::Info() << name_ << " cancel request -> " << request->Url();
			return code;
		}
		if (sibling_) {
			code = sibling_->Process(route, request, response);
		} else if (route) {
			response->SetStatusCode(200);
			code = route->Process(request, response);
		}
		OnProcessDone(request, response);
		return code;
	}

	int HttpMiddleware::OnProcessDone(shared_ptr<HttpRequest> request, shared_ptr<HttpResponse> response) {
		return 0;
	}
} // namespace simple

