#include "server.h"


namespace simple {
	IoEventHandler::~IoEventHandler() {
	}

	Server::Server(shared_ptr<IoEventHandler> io_event_handler)
		: io_event_handler_(io_event_handler) {
	}

	Server::~Server() {
	}

	shared_ptr<IoEventHandler> Server::GetIoEventHandler() const {
		return io_event_handler_;
	}
} // namespace simple


