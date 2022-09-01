#include <memory>
#include "utils/command_line.h"


using std::shared_ptr;
extern int ServiceMain(const string& name, shared_ptr<simple::CommandLine> commandline);
int main(int argc, char* argv[]) {
	int exit_code = 0;
	auto commandline = std::make_shared<simple::CommandLine>(argc, argv);
	exit_code = ServiceMain("simple", commandline);
	return exit_code;
}

