#include "routes.h"

#include "route.h"


namespace simple {
	void HttpRoutes::Install(shared_ptr<HttpRoute> route) {
		string path = route->Path();
		if (path.empty())
			return;
		if (path.at(0) == '~') {
			pattern_routes_.push_back(route);
		} else {
			normal_routes_[route->Path()] = route;
		}
	}

	shared_ptr<HttpRoute> HttpRoutes::Find(const string& path, unordered_map<string, string>& args) {
		auto it = normal_routes_.find(path);
		if (it != normal_routes_.end())
			return it->second;
		for (auto it = pattern_routes_.begin(); it != pattern_routes_.end(); ++it) {
			string route_path = (*it)->Path();
			string pattern = route_path.substr(1);
			if ((*it)->Match(pattern, path, args)) {
				return *it;
			}
		}
		return nullptr;
	}
} // namespace simple

