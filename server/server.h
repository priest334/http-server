#pragma once
#ifndef SERVER_SERVER_H_
#define SERVER_SERVER_H_

#include <memory>
#include <string>
#include <vector>
#include "utils/task.h"
#include "buffer.h"
#include "client_session.h"


using std::shared_ptr;
using std::string;
using std::vector;
namespace simple {
	class IoEventHandler {
	public:
		virtual ~IoEventHandler();
		virtual int OnCreate(shared_ptr<ClientSession> session) = 0;
		virtual int OnRead(shared_ptr<ClientSession> session, shared_ptr<read_buffer_t> rdbuf, int size) = 0;
		virtual int OnWrite(shared_ptr<ClientSession> session, shared_ptr<write_buffer_t> wrbuf, int status) = 0;
		virtual int OnClose(shared_ptr<ClientSession> session) = 0;
		virtual int OnTimeout(shared_ptr<ClientSession> session, int how) = 0;
	};

	class Server {
	public:
		Server(shared_ptr<IoEventHandler> io_event_handler);
		virtual ~Server();
		shared_ptr<IoEventHandler> GetIoEventHandler() const;
		virtual int Start(const string& ip, int port) = 0;
		virtual int Stop() = 0;
		virtual int Write(shared_ptr<ClientSession>, shared_ptr<write_buffer_t> wrbuf) = 0;
		virtual int Close(shared_ptr<ClientSession> session) = 0;
		virtual int AsyncExec(shared_ptr<Task> task) = 0;
	protected:
		shared_ptr<IoEventHandler> io_event_handler_;
	};
} // namespace hlp

#endif // SERVER_SERVER_H_

