#include "config_item.h"

#include "stl_helper.h"
#include "config_parser.h"


namespace simple {
	ConfigItem::ConfigItem(const string& name)
		: name_(name.c_str()) {
	}

	string ConfigItem::Name() const {
		return name_;
	}

	shared_ptr<ConfigItem> ConfigItem::GetChild(const string& name) const {
		auto it = children_.find(name);
		if (it != children_.end()) {
			return it->second;
		}
		return nullptr;
	}

	string ConfigItem::GetProperty(const string& property_name) const {
		return MapValue(properties_, property_name);
	}

	bool ConfigItem::HasProperty(const string& property_name) const {
		auto it = properties_.find(property_name);
		return (it != properties_.end());
	}

	void ConfigItem::SetProperty(const string& property_name, const string& property_value) {
		properties_[property_name] = property_value;
	}

	void ConfigItem::Append(const string& name, shared_ptr<ConfigItem> child) {
		children_[name] = child;
	}

	const unordered_map<string, string>& ConfigItem::Properties() const {
		return properties_;
	}

	const unordered_map<string, shared_ptr<ConfigItem>>& ConfigItem::Children() const {
		return children_;
	}
} // namespace simple

