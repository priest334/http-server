#include "stl_helper.h"


namespace simple {
	string MapDefautValue(const unordered_map<string, string>& m, const string& key, const string& defualt) {
		if (key.empty())
			return defualt;
		const auto& it = m.find(key);
		if (it != m.end()) {
			return it->second;
		}
		return defualt;
	}

	string MapValue(const unordered_map<string, string>& m, const string& key) {
		return MapDefautValue(m, key, string());
	}
} // namespace simple


