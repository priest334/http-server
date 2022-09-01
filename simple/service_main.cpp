#include <cstdlib>
#include <memory>
#include <string>
#include <sstream>
#include "utils/command_line.h"
#include "utils/config.h"
#include "utils/daemonize.h"
#include "utils/logger.h"
#include "service.h"
#ifdef _WIN32
#define PutEnv(str) _putenv(str)
#else
#define PutEnv(str) putenv((char*)str)
#endif


using std::shared_ptr;
using std::string;
using std::ostringstream;

int ServiceMain(const string& name, shared_ptr<simple::CommandLine> commandline) {
	if (commandline->HasSwitch("daemon")) {
		simple::Daemonize();
	}
	string config_file = commandline->GetSwitchValue("config");
	if (config_file.empty())
		config_file = name + ".conf";
	auto config = std::make_shared<simple::Config>();
	config->Load(config_file);
	string logfile = config->Get("log.file");
	if (logfile.empty())
		logfile = name + ".log";
	simple::logger::SetWriter(std::make_shared<simple::SingleFile>(logfile));
	string ip = config->Get("listener.ip");
	int port = (int)config->GetInt("listener.port");
	if (ip.empty() || port == 0) {
		simple::logger::Fatal() << "listener.ip or listener.port invalid";
		return 0;
	}
	int threads = (int)config->GetInt("server.workers", (int64_t)4);
	if (threads > 0) {
		ostringstream oss;
		oss << "UV_THREADPOOL_SIZE=" << threads;
		int retval = PutEnv(oss.str().c_str());
		if (0 != retval) {
			simple::logger::Error() << "setenv UV_THREADPOOL_SIZE failed: " << errno;
		}
	}
	auto server = std::make_shared<HttpService>(config);
	server->Start(ip, port);
	return 0;
}
