#pragma once
#ifndef UTILS_CONFIG_H_
#define UTILS_CONFIG_H_

#include <cstdint>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>


using std::ostringstream;		
using std::shared_ptr;
using std::string;
using std::unordered_map;
namespace simple {
	class ConfigItem;
	class ConfigParser;

	class Config {
		string Get(shared_ptr<ConfigItem> parent, const string& name) const;
		bool Set(shared_ptr<ConfigItem> parent, const string& name, const string& value);
	public:
		Config();
		bool Load(const string& filename);
		bool Save(const string& filename);
		shared_ptr<ConfigItem> Root() const;

		string Get(const string& name) const;
		string Get(const string& name, const string& default_value);
		int64_t GetInt(const string& name) const;
		int64_t GetInt(const string& name, int64_t default_value) const;
		bool Set(const string& name, const string& value);
		bool SetInt(const string& name, int64_t value);
		unordered_map<string, string> GetAll(const string& name) const;
	private:
		shared_ptr<ConfigParser> parser_;
		shared_ptr<ConfigItem> root_;
	};
} // namespace simple

#endif // UTILS_CONFIG_H_

