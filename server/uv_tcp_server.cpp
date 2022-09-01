#include "uv_tcp_server.h"

#include <iostream>
#include <sstream>
#include <unordered_map>
#include "utils/logger.h"
#include "utils/simple_queue.h"
#include "utils/uuid_helper.h"
#include "tcp_server.h"


namespace simple {
	namespace {
		void PrintError(const string& what, const string& message) {
			logger::Error() << what << ": " << message;
		}
		class UvTcpSessionContext {
		public:
			UvTcpSessionContext(shared_ptr<UvTcpServer> server)
				: server_(server),
				rdbuf_(std::make_shared<read_buffer_t>()) {
				uv_handle_set_data((uv_handle_t*)&handle_, this);
				uv_tcp_nodelay(&handle_, 1);
				uv_tcp_keepalive(&handle_, 1, 60);
			}

			shared_ptr<UvTcpServer> server() {
				return server_;
			}

			uv_tcp_t& handle() {
				return handle_;
			}

			shared_ptr<read_buffer_t> rdbuf() {
				return rdbuf_;
			}
		private:
			shared_ptr<UvTcpServer> server_;
			shared_ptr<read_buffer_t> rdbuf_;
			uv_tcp_t handle_;
		};
	}

	class WriteTask {
	public:
		WriteTask(shared_ptr<UvTcpServer> server, shared_ptr<ClientSession> session, shared_ptr<write_buffer_t> wrbuf)
			: server_(server), session_(session), wrbuf_(wrbuf) {
			uv_handle_set_data((uv_handle_t*)&handle_, this);
		}

		string Id() const {
			return id_.ToString();
		}

		shared_ptr<UvTcpServer> server() {
			return server_;
		}

		shared_ptr<ClientSession> session() {
			return session_;
		}

		shared_ptr<write_buffer_t> wrbuf() {
			return wrbuf_;
		}

		uv_write_t& handle() {
			return handle_;
		}
	private:
		Uuid id_;
		shared_ptr<UvTcpServer> server_;
		shared_ptr<ClientSession> session_;
		shared_ptr<write_buffer_t> wrbuf_;
		uv_write_t handle_;
	};

	class AsyncTask {
	public:
		AsyncTask(shared_ptr<UvTcpServer> server, shared_ptr<Task> task)
			: server_(server), task_(task) {
			uv_handle_set_data((uv_handle_t*)&handle_, this);
		}

		string Id() const {
			return id_.ToString();
		}

		shared_ptr<UvTcpServer> server() {
			return server_;
		}

		shared_ptr<Task> task() {
			return task_;
		}

		uv_work_t& handle() {
			return handle_;
		}
	private:
		Uuid id_;
		shared_ptr<UvTcpServer> server_;
		shared_ptr<Task> task_;
		uv_work_t handle_;
	};

	UvTcpServer* UvTcpServer::ServerFromHandle(const uv_handle_t* handle) {
		uv_loop_t* loop = uv_handle_get_loop(handle);
		UvTcpServer* server = static_cast<UvTcpServer*>(uv_loop_get_data(loop));
		return server;
	}

	void UvTcpServer::TimerCallback(uv_timer_t* handle) {
		auto server = ServerFromHandle((const uv_handle_t*)handle);
		if (server) {
			server->OnTimer();
		}
	}

	void UvTcpServer::AsyncCallback(uv_async_t* handle) {
		auto server = ServerFromHandle((const uv_handle_t*)handle);
		if (server) {
			server->DoWrite();
		}
	}

	void UvTcpServer::AcceptCallback(uv_stream_t* listener, int status) {
		if (status < 0) {
			return;
		}
		auto server = ServerFromHandle((const uv_handle_t*)listener);
		if (!server) {
			return;
		}
		server->DoAccept();
	}

	void UvTcpServer::AllocCallback(uv_handle_t* client, size_t suggested_size, uv_buf_t* buf) {
		ClientSession* session = (ClientSession*)uv_handle_get_data(client);
		auto context = std::static_pointer_cast<UvTcpSessionContext>(session->Context());
		if (context) {
			auto rdbuf = context->rdbuf();
			if (rdbuf->size() < suggested_size) {
				rdbuf->resize(suggested_size);
			}
			buf->base = rdbuf->data();
			buf->len = suggested_size;
		}
	}

	void UvTcpServer::ReadCallback(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {
		ClientSession* session = (ClientSession*)uv_handle_get_data((uv_handle_t*)client);
		auto context = std::static_pointer_cast<UvTcpSessionContext>(session->Context());
		if (nread > 0) {
			session->OnRead(context->rdbuf(), nread);
		} else {
#ifdef _WIN32
			if (WSAEWOULDBLOCK != WSAGetLastError()) {
#else
			if (errno != EAGAIN) {
#endif
				if (nread != UV_EOF) {
					PrintError("uv_read_cb", uv_err_name(nread));
				}
				auto server = context->server();
				if (server) {
					server->Close(session->Id());
				}
			}
		}
	}

	void UvTcpServer::WriteCallback(uv_write_t* req, int status) {
		WriteTask* write_task = (WriteTask*)uv_handle_get_data((uv_handle_t*)req);
		auto session = write_task->session();
		session->OnWrite(write_task->wrbuf(), status);
		auto server = write_task->server();
		if (session->IsClosing() && session->WriteQueueSize() <=0) {
			auto context = std::static_pointer_cast<UvTcpSessionContext>(write_task->session()->Context());
			uv_read_stop((uv_stream_t*)&context->handle());
			if (!uv_is_closing((uv_handle_t*)&context->handle())) {
				uv_close((uv_handle_t*)&context->handle(), &UvTcpServer::CloseCallback);
			}
		}
		server->WriteClean(write_task->Id());
	}

	void UvTcpServer::CloseCallback(uv_handle_t* client) {
		ClientSession* session = (ClientSession*)uv_handle_get_data((uv_handle_t*)client);
		if (!session)
			return;
		auto context = std::static_pointer_cast<UvTcpSessionContext>(session->Context());
		if (context) {
			auto server = context->server();
			if (server) {
				server->Close(session->Id());
			}
		}
	}

	void UvTcpServer::WorkCallback(uv_work_t* work) {
		AsyncTask* async_task = (AsyncTask*)uv_handle_get_data((const uv_handle_t*)work);
		if (async_task) {
			auto task = async_task->task();
			if (task) {
				task->Run();
			}
		}
	}

	void UvTcpServer::AfterWorkCallback(uv_work_t* work, int status) {
		AsyncTask* async_task = (AsyncTask*)uv_handle_get_data((const uv_handle_t*)work);
		if (async_task) {
			auto task = async_task->task();
			if (task) {
				task->Done();
			}
			auto server = async_task->server();
			server->TaskClean(async_task->Id());
		}
	}


	UvTcpServer::UvTcpServer(shared_ptr<TcpServer> server)
		: server_(server) {
		uv_loop_init(&loop_);		
		uv_loop_set_data(&loop_, this);
		uv_async_init(&loop_, &async_, &UvTcpServer::AsyncCallback);
		uv_tcp_init(&loop_, &listener_);
		uv_mutex_init(&task_queue_mutex_);
		uv_handle_set_data((uv_handle_t*)&timer_, this);
		uv_timer_init(&loop_, &timer_);
	}

	UvTcpServer::~UvTcpServer() {
		uv_mutex_destroy(&task_queue_mutex_);
		uv_close((uv_handle_t*)&listener_, nullptr);
		uv_loop_close(&loop_);
	}

	int UvTcpServer::Start(const string& ip, int port) {
		struct sockaddr_in addr;
		int retval = uv_ip4_addr(ip.c_str(), port, &addr);
		if (retval != 0) {
			PrintError("uv_ip4_addr", uv_strerror(retval));
			return -1;
		}
		retval = uv_tcp_init(&loop_, &listener_);
		if (retval != 0) {
			PrintError("uv_tcp_init", uv_strerror(retval));
			return -1;
		}
		uv_stream_set_blocking((uv_stream_t*)&listener_, 0);
		retval = uv_tcp_bind(&listener_, (const sockaddr*)&addr, 0);
		if (retval != 0) {
			PrintError("uv_tcp_bind", uv_strerror(retval));
			return -1;
		}
		retval = uv_listen((uv_stream_t*)&listener_, SOMAXCONN, &UvTcpServer::AcceptCallback);
		if (retval != 0) {
			PrintError("uv_listen", uv_strerror(retval));
			return -1;
		}
		uv_timer_start(&timer_, &UvTcpServer::TimerCallback, 1000, 1000);
		uv_run(&loop_, UV_RUN_DEFAULT);
		return 0;
	}

	int UvTcpServer::Stop() {
		uv_stop(&loop_);
		return 0;
	}

	int UvTcpServer::Write(shared_ptr<ClientSession> session, shared_ptr<write_buffer_t> wrbuf) {
		auto write_task = std::make_shared<WriteTask>(shared_from_this(), session, wrbuf);
		wrlist_.Push(write_task);
		uv_async_send(&async_);
		return 0;
	}

	int UvTcpServer::Close(shared_ptr<ClientSession> session) {
		return Write(session, nullptr);
	}

	int UvTcpServer::Close(const string& id) {
		auto server = GetServer();
		if (!server) {
			return 0;
		}
		server->CloseSession(id);
		return 0;
	}

	int UvTcpServer::AsyncExec(shared_ptr<Task> task) {
		auto async_task = std::make_shared<AsyncTask>(shared_from_this(), task);
		uv_mutex_lock(&task_queue_mutex_);
		async_task_list_[async_task->Id()] = async_task;
		uv_queue_work(&loop_, &async_task->handle(), &UvTcpServer::WorkCallback, &UvTcpServer::AfterWorkCallback);
		uv_mutex_unlock(&task_queue_mutex_);
		return 0;
	}

	shared_ptr<TcpServer> UvTcpServer::GetServer() {
		auto server = server_.lock();
		return server;
	}

	void UvTcpServer::DoAccept() {
		auto server = GetServer();
		if (!server) {
			return;
		}
		auto context = std::make_shared<UvTcpSessionContext>(shared_from_this());
		uv_tcp_init(&loop_, &context->handle());
		auto session = server->CreateSession(context);
		uv_handle_set_data((uv_handle_t*)(&context->handle()), session.get());
		int retval = uv_accept((uv_stream_t*)&listener_, (uv_stream_t*)&context->handle());
		if (0 != retval) {
			PrintError("uv_accept", uv_strerror(retval));
			session->Close();
			return;
		}
		auto io_event_handler = server->GetIoEventHandler();
		if (io_event_handler) {
			io_event_handler->OnCreate(session);
		}
		uv_read_start((uv_stream_t*)&context->handle(), &UvTcpServer::AllocCallback, &UvTcpServer::ReadCallback);
	}

	void UvTcpServer::DoWrite() {
		auto write_task_list = wrlist_.Get(-1, true);
		for (auto write_task : *write_task_list) {
			auto context = std::static_pointer_cast<UvTcpSessionContext>(write_task->session()->Context());
			if (!write_task->wrbuf()) {
				auto session = write_task->session();
				session->SetClosing();
				if (session->WriteQueueSize() <= 0) {
					uv_read_stop((uv_stream_t*)&context->handle());
					if (!uv_is_closing((uv_handle_t*)&context->handle())) {
						uv_close((uv_handle_t*)&context->handle(), &UvTcpServer::CloseCallback);
					}
				}
			} else {
				written_[write_task->Id()] = write_task;
				uv_buf_t buffer;
				buffer.base = write_task->wrbuf()->data();
				buffer.len = write_task->wrbuf()->size();
				uv_write(&write_task->handle(), (uv_stream_t*)&context->handle(), &buffer, 1, &UvTcpServer::WriteCallback);
			}
		}
	}

	void UvTcpServer::WriteClean(const string& id) {
		written_.erase(id);
	}

	void UvTcpServer::TaskClean(const string& id) {
		uv_mutex_lock(&task_queue_mutex_);
		async_task_list_.erase(id);
		uv_mutex_unlock(&task_queue_mutex_);
	}

	void UvTcpServer::OnTimer() {
		auto server = GetServer();
		if (!server) {
			return;
		}
		server->OnTimer();
	}
} // namespace simple


