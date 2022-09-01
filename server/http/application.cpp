#include "application.h"

#include "route.h"
#include "routes.h"
#include "middleware.h"
#include "request.h"
#include "response.h"


namespace simple {
	HttpApplication::HttpApplication()
		: routes_(std::make_shared<HttpRoutes>()) {
	}

	HttpApplication::~HttpApplication() {
	}

	string HttpApplication::Name() const {
		return "simple/1.0";
	}

	void HttpApplication::InstallRoute(shared_ptr<HttpRoute> route) {
		if (!route)
			return;
		routes_->Install(route);
	}

	void HttpApplication::InstallMiddleware(shared_ptr<HttpMiddleware> middleware) {
		if (!middleware)
			return;
		if (!middleware_) {
			middleware_ = middleware;
		} else {
			middleware_->Append(middleware);
		}
	}

	int HttpApplication::Process(shared_ptr<HttpRequest> request, shared_ptr<HttpResponse> response) {
		auto route = routes_->Find(request->Path(), request->PathArgList());
		if (!route) {
			return -1;
		}
		if (!route->MatchVerb(request->Method())) {
			response->SetStatusCode(405);
			return -1;
		}
		if (middleware_) {
			middleware_->Process(route, request, response);
		} else {
			response->SetStatusCode(200);
			route->Process(request, response);
		}
		return 0;
	}
} // namespace simple

