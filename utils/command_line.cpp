#include "command_line.h"

#include <cstring>
#include "stl_helper.h"


namespace simple {
	CommandLine::CommandLine() {
	}

	CommandLine::CommandLine(int argc, char* argv[]) {
		FromArgs(argc, argv);
	}

	void CommandLine::FromArgs(int argc, char* argv[]) {
		application_ = argv[0];
		for (int i = 1; i < argc; i++) {
			char* argp = argv[i];
			for (char ch = *argp; ch == '-'; ch = *++argp)
				continue;
			char* sep = strchr(argp, '=');
			if (sep) {
				string name(argp, sep - argp);
				string value(sep + 1);
				switches_[name] = value;
			} else {
				switches_[argp] = "";
			}
		}
	}

	bool CommandLine::HasSwitch(const string& name) {
		return (switches_.end() != switches_.find(name));
	}

	string CommandLine::GetSwitchValue(const string& name) {
		return MapValue(switches_, name);
	}

	string CommandLine::GetSwitchValue(const string& name, const string& default_value) {
		return MapDefautValue(switches_, name, default_value);
	}
} // namespace simple


