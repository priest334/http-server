#include "client_session.h"

#include <ctime>
#include "utils/uuid_helper.h"
#include "tcp_server.h"


namespace simple {
	ClientSession::ClientSession(shared_ptr<Server> server, shared_ptr<void> context)
		: id_(Uuid().ToString()),
		server_(server),
		context_(context),
		read_timeout_(-1),
		timestamp_last_read_(time(nullptr)),
		write_timeout_(-1),
		timestamp_last_write_(-1),
		closing_(false),
		write_queue_size_(0) {
	}

	string ClientSession::Id() const {
		return id_;
	}

	shared_ptr<void> ClientSession::Context() {
		return context_;
	}

	shared_ptr<void> ClientSession::Context() const {
		return context_;
	}

	void ClientSession::SetUserContext(shared_ptr<void> user_context) {
		user_context_ = user_context;
	}

	shared_ptr<void> ClientSession::UserContext() {
		return user_context_;
	}

	shared_ptr<void> ClientSession::UserContext() const {
		return user_context_;
	}

	int ClientSession::OnRead(shared_ptr<read_buffer_t> rdbuf, int size) {
		std::atomic_store(&timestamp_last_read_, (int64_t)time(nullptr));
		std::atomic_store(&timestamp_last_write_, (int64_t)time(nullptr));
		auto server = server_.lock();
		if (!server) {
			return 0;
		}
		auto io_event_handler = server->GetIoEventHandler();
		if (io_event_handler) {
			return io_event_handler->OnRead(shared_from_this(), rdbuf, size);
		}
		return 0;
	}

	int ClientSession::OnWrite(shared_ptr<write_buffer_t> wrbuf, int status) {
		std::atomic_fetch_sub(&write_queue_size_, 1);
		std::atomic_store(&timestamp_last_write_, (int64_t)time(nullptr));
		auto server = server_.lock();
		if (!server) {
			return 0;
		}
		auto io_event_handler = server->GetIoEventHandler();
		if (io_event_handler) {
			return io_event_handler->OnWrite(shared_from_this(), wrbuf, status);
		}
		return 0;
	}

	int ClientSession::OnTimeout(int how) {
		auto server = server_.lock();
		if (!server) {
			return 0;
		}
		auto io_event_handler = server->GetIoEventHandler();
		if (io_event_handler) {
			return io_event_handler->OnTimeout(shared_from_this(), how);
		}
		return 0;
	}

	void ClientSession::Write(shared_ptr<write_buffer_t> wrbuf) {
		std::atomic_fetch_add(&write_queue_size_, 1);
		auto server = server_.lock();
		if (!server) {
			return;
		}
		server->Write(shared_from_this(), wrbuf);
	}

	int ClientSession::WriteQueueSize() const {
		return std::atomic_load(&write_queue_size_);
	}

	void ClientSession::SetClosing() {
		std::atomic_store(&closing_, true);
	}

	bool ClientSession::IsClosing() const {
		return std::atomic_load(&closing_);
	}

	void ClientSession::Close() {
		auto server = server_.lock();
		if (!server) {
			return;
		}
		server->Close(shared_from_this());
	}

	void ClientSession::SetTimeout(int seconds, int how/* = 0x3*/) {
		if (how & 0x1) {
			read_timeout_ = seconds;
		}
		if (how & 0x2) {
			write_timeout_ = seconds;
		}
	}

	int ClientSession::IsTimeout(int64_t now, int how/* = 0x3*/) const {
		if ((how & 0x1) == 0x1 && read_timeout_ > 0) {
			return (std::atomic_load(&timestamp_last_read_) + read_timeout_ < now) ? 0x1 : 0x0;
		}
		if ((how & 0x2) == 0x2 && write_timeout_ > 0) {
			return (std::atomic_load(&timestamp_last_write_) + write_timeout_ < now) ? 0x2 : 0x0;
		}
		return 0x0;
	}

	int ClientSession::IsTimeout(int how/* = 0x3*/) const {
		return IsTimeout((int64_t)time(nullptr), how);
	}
} // namespace hlp


