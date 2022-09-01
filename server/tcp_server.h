#pragma once
#ifndef SERVER_TCP_SERVER_H_
#define SERVER_TCP_SERVER_H_

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include "server.h"


using std::mutex;
using std::shared_ptr;
using std::string;
using std::unordered_map;
using std::vector;
namespace simple {
	class ServerSettings;
	class UvTcpServer;
	class TcpServer
		: public Server,
		public std::enable_shared_from_this<TcpServer> {
		friend class UvTcpServer;
	public:
		TcpServer(shared_ptr<IoEventHandler> io_event_handler);
		TcpServer(shared_ptr<IoEventHandler> io_event_handler, shared_ptr<ServerSettings> settings);
		~TcpServer();
		int Start(const string& ip, int port) override;
		int Stop() override;
		int Write(shared_ptr<ClientSession> session, shared_ptr<write_buffer_t> wrbuf) override;
		int Close(shared_ptr<ClientSession> session) override;
		int AsyncExec(shared_ptr<Task> task) override;
		shared_ptr<ClientSession> CreateSession(shared_ptr<void> context);
		void CloseSession(const string& id);
		bool IsValidSession(const string& id) const;
		void OnTimer();
	protected:
		shared_ptr<ServerSettings> settings_;
		shared_ptr<UvTcpServer> impl_;
		mutable mutex session_list_mutex_;
		unordered_map<string, shared_ptr<ClientSession>> session_list_;
	};
} // namespace simple

#endif // SERVER_TCP_SERVER_H_

