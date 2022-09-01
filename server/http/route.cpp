#include "route.h"

#include <regex>
#include <vector>


using std::vector;
namespace simple {
	namespace {
		bool MatchPathArgs(const string& pattern, const string& path, unordered_map<string, string>& args) {
			string expr;
			std::vector<string> keys;
			for (size_t i = 0; i < pattern.length(); i++) {
				char ch = pattern.at(i);
				if (ch == '{') {
					size_t j = i + 1, k = 0, n = 0;
					for (; j < pattern.length(); j++) {
						char ch1 = pattern.at(j);
						if (ch1 == '{') {
							n++;
						} else if (ch1 == '}') {
							if (n == 0) {
								if (k > 0 && k < j) {
									string value = pattern.substr(k, j - k);
									expr.append("(" + value + ")");
								} else {
									string name = pattern.substr(i + 1, j - i - 1);
									keys.push_back(name);
									expr.append("([^/\\?]+)");
								}
								i = j;
								break;
							}
							n--;
						}
						if (ch1 == ':') {
							string name = pattern.substr(i + 1, j - i - 1);
							keys.push_back(name);
							k = j + 1;
						}
					}
				} else {
					expr.append(1, ch);
				}
			}
			std::cmatch mr;
			std::regex express(expr);
			if (std::regex_match(path.c_str(), mr, express)) {
				for (size_t i = 0; i < keys.size(); i++) {
					args[keys[i]] = mr[i + 1];
				}
				return true;
			}
			return false;
		}
	}

	HttpRoute::~HttpRoute() {
	}

	bool HttpRoute::MatchVerb(const string& verb) const {
		return true;
	}

	bool HttpRoute::Match(const string& pattern, const string& path, unordered_map<string, string>& args) const {
		if (MatchPathArgs(pattern, path, args)) {
			return true;
		}
		return false;
	}
} // namespace simple

