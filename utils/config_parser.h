#pragma once
#ifndef UTILS_CONFIG_PARSER_H_
#define UTILS_CONFIG_PARSER_H_

#include <string>


using std::string;
namespace simple {
	class ConfigItem;

	class ConfigParser {
	public:
		virtual ~ConfigParser();
		virtual bool Load(const string& text) = 0;
		virtual string Save(ConfigItem* root) const = 0;
	};
} // namespace simple

#endif // UTILS_CONFIG_PARSER_H_

