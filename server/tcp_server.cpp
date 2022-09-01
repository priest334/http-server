#include "tcp_server.h"

#include "utils/logger.h"
#include "uv_tcp_server.h"


namespace simple {
	TcpServer::TcpServer(shared_ptr<IoEventHandler> io_event_handler)
		: Server(io_event_handler), settings_(nullptr) {
	}

	TcpServer::TcpServer(shared_ptr<IoEventHandler> io_event_handler, shared_ptr<ServerSettings> settings)
		: Server(io_event_handler), settings_(settings) {
	}

	TcpServer::~TcpServer() {
	}

	int TcpServer::Start(const string& ip, int port) {
		if (impl_)
			return -1;
		impl_ = std::make_shared<UvTcpServer>(shared_from_this());
		return impl_->Start(ip, port);
	}

	int TcpServer::Stop() {
		if (!impl_) {
			return -1;
		}
		return impl_->Stop();
	}

	int TcpServer::Write(shared_ptr<ClientSession> session, shared_ptr<write_buffer_t> wrbuf) {
		if (!impl_) {
			return -1;
		}
		if (!IsValidSession(session->Id())) {
			logger::Warning() << session->Id() << ", write failed";
			return -2;
		}
		return impl_->Write(session, wrbuf);
	}

	int TcpServer::Close(shared_ptr<ClientSession> session) {
		if (!impl_) {
			return -1;
		}
		return impl_->Close(session);
	}

	int TcpServer::AsyncExec(shared_ptr<Task> task) {
		if (!impl_) {
			return -1;
		}
		return impl_->AsyncExec(task);
	}

	shared_ptr<ClientSession> TcpServer::CreateSession(shared_ptr<void> context) {
		auto session = std::make_shared<ClientSession>(shared_from_this(), context);
		std::lock_guard<std::mutex> lock(session_list_mutex_);
		session_list_[session->Id()] = session;
		return session;
	}

	void TcpServer::CloseSession(const string& id) {
		shared_ptr<ClientSession> session;
		do {
			std::lock_guard<std::mutex> lock(session_list_mutex_);
			auto it = session_list_.find(id);
			if (it != session_list_.end()) {
				session = it->second;
				session_list_.erase(id);
			}
		} while (0);
		if (io_event_handler_ && session) {
			io_event_handler_->OnClose(session);
		}
	}

	bool TcpServer::IsValidSession(const string& id) const {
		std::lock_guard<std::mutex> lock(session_list_mutex_);
		auto it = session_list_.find(id);
		return it != session_list_.end();
	}

	void TcpServer::OnTimer() {
		unordered_map<string, shared_ptr<ClientSession>> sessions;
		do {
			std::lock_guard<std::mutex> lock(session_list_mutex_);
			sessions.insert(session_list_.begin(), session_list_.end());
		} while (0);
		int64_t now = (int64_t)time(nullptr);
		for (const auto& it : sessions) {
			int how = it.second->IsTimeout(now, 0x3);
			if (how != 0) {
				it.second->OnTimeout(how);
			}
		}
	}
} // namespace simple


