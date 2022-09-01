#pragma once
#ifndef UTILS_CONFIG_ITEM_H_
#define UTILS_CONFIG_ITEM_H_

#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>


using std::shared_ptr;
using std::string;
using std::unordered_map;
namespace simple {
	class ConfigParser;

	class ConfigItem {
	public:
		ConfigItem(const string& name);
		string Name() const;
		shared_ptr<ConfigItem> GetChild(const string& name) const;
		string GetProperty(const string& property_name) const;
		bool HasProperty(const string& property_name) const;
		void SetProperty(const string& property_name, const string& property_value);
		void Append(const string& name, shared_ptr<ConfigItem> child);
		const unordered_map<string, string>& Properties() const;
		const unordered_map<string, shared_ptr<ConfigItem>>& Children() const;
	private:
		string name_;
		unordered_map<string, string> properties_;
		unordered_map<string, shared_ptr<ConfigItem>> children_;
	};
} // namespace simple

#endif // UTILS_CONFIG_ITEM_H_

