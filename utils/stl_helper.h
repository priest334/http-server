#pragma once
#ifndef UTILS_STL_HELPER_H_
#define UTILS_STL_HELPER_H_

#include <string>
#include <unordered_map>


using std::string;
using std::unordered_map;
namespace simple {
	string MapDefautValue(const unordered_map<string, string>& m, const string& key, const string& defualt);
	string MapValue(const unordered_map<string, string>& m, const string& key);
} // namespace simple

#endif // UTILS_STL_HELPER_H_

