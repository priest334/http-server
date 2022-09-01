#pragma once
#ifndef UTILS_COMMAND_LINE_H_
#define UTILS_COMMAND_LINE_H_

#include <string>
#include <unordered_map>


using std::string;
using std::unordered_map;
namespace simple {
	class CommandLine {
	public:
		CommandLine();
		CommandLine(int argc, char* argv[]);
		void FromArgs(int argc, char* argv[]);
		bool HasSwitch(const string& name);
		string GetSwitchValue(const string& name);
		string GetSwitchValue(const string& name, const string& default_value);
	private:
		string application_;
		unordered_map<string, string> switches_;
	};
} // namespace hlp

#endif // UTILS_COMMAND_LINE_H_

