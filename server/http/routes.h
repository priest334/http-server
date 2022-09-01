#pragma once
#ifndef SERVER_HTTP_ROUTES_H_
#define SERVER_HTTP_ROUTES_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


using std::shared_ptr;
using std::string;
using std::unordered_map;
using std::vector;
namespace simple {
	class HttpRoute;

	class HttpRoutes {
		typedef unordered_map<string, shared_ptr<HttpRoute>> NormalRoutes;
		typedef vector<shared_ptr<HttpRoute>> PatternRoutes;
	public:
		void Install(shared_ptr<HttpRoute> route);
		shared_ptr<HttpRoute> Find(const string& path, unordered_map<string,string>& args);
	private:
		NormalRoutes normal_routes_;
		PatternRoutes pattern_routes_;
	};
} // namespace hlp

#endif // SERVER_HTTP_ROUTES_H_

