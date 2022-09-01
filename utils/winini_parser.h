#pragma once
#ifndef UTILS_WININI_PARSER_H_
#define UTILS_WININI_PARSER_H_

#include "config_parser.h"


namespace simple {
	class Config;

	class WinIniParser 
		: public ConfigParser {
	public:
		WinIniParser(Config* config);
		~WinIniParser();
		bool Load(const string& text) override;
		string Save(ConfigItem* root) const override;
	private:
		Config* config_;
	};
} // namespace simple

#endif // UTILS_WININI_PARSER_H_

