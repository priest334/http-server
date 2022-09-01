#pragma once
#ifndef SERVER_HTTP_ROUTE_H_
#define SERVER_HTTP_ROUTE_H_

#include <memory>
#include <string>
#include <unordered_map>


using std::shared_ptr;
using std::string;
using std::unordered_map;
namespace simple {
	class HttpRequest;
	class HttpResponse;

	class HttpRoute {
	public:
		virtual ~HttpRoute();
		virtual bool MatchVerb(const string& verb) const;
		virtual bool Match(const string& pattern, const string& path, unordered_map<string, string>& args) const;
		virtual string Path() const = 0;
		virtual int Process(shared_ptr<HttpRequest> request, shared_ptr<HttpResponse> response) = 0;
	};
} // namespace simple

#endif // SERVER_HTTP_ROUTE_H_

