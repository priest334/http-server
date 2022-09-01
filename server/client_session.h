#pragma once
#ifndef SERVER_CLIENT_SESSION_H_
#define SERVER_CLIENT_SESSION_H_

#include <atomic>
#include <memory>
#include <string>
#include <vector>
#include "buffer.h"


using std::shared_ptr;
using std::string;
using std::vector;
using std::weak_ptr;
namespace simple {
	class Server;
	class ClientSession
		: public std::enable_shared_from_this<ClientSession> {
	public:
		ClientSession(shared_ptr<Server> server, shared_ptr<void> context);
		string Id() const;
		shared_ptr<void> Context();
		shared_ptr<void> Context() const;
		void SetUserContext(shared_ptr<void> user_context);
		shared_ptr<void> UserContext();
		shared_ptr<void> UserContext() const;
		int OnRead(shared_ptr<read_buffer_t> rdbuf, int size);
		int OnWrite(shared_ptr<write_buffer_t> wrbuf, int status);
		int OnTimeout(int how);
		void Write(shared_ptr<write_buffer_t> wrbuf);
		int WriteQueueSize() const;
		void SetClosing();
		bool IsClosing() const;
		void Close();
		void SetTimeout(int seconds, int how = 0x3);
		int IsTimeout(int64_t now, int how = 0x3) const;
		int IsTimeout(int how = 0x3) const;
	private:
		string id_;
		weak_ptr<Server> server_;
		shared_ptr<void> context_;
		shared_ptr<void> user_context_;
		int64_t read_timeout_;
		std::atomic_int64_t timestamp_last_read_;
		int64_t write_timeout_;
		std::atomic_int64_t timestamp_last_write_;
		std::atomic_bool closing_;
		mutable std::atomic_int write_queue_size_;
	};
} // namespace simple

#endif // SERVER_CLIENT_SESSION_H_

