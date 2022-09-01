#pragma once
#ifndef SERVER_TCP_UV_TCP_SERVER_H_
#define SERVER_TCP_UV_TCP_SERVER_H_

#include <memory>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>
#include <uv.h>
#include <utils/task.h>
#include "utils/simple_queue.h"
#include "client_session.h"


using std::deque;
using std::shared_ptr;
using std::string;
using std::unordered_map;
using std::vector;
using std::weak_ptr;
namespace simple {
	class AsyncTask;
	class WriteTask;
	class TcpServer;
	class UvTcpServer
		: public std::enable_shared_from_this<UvTcpServer> {
		static UvTcpServer* ServerFromHandle(const uv_handle_t* handle);
		static void TimerCallback(uv_timer_t* handle);
		static void AsyncCallback(uv_async_t* handle);
		static void AcceptCallback(uv_stream_t* listener, int status);
		static void AllocCallback(uv_handle_t* client, size_t suggested_size, uv_buf_t* buf);
		static void ReadCallback(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
		static void WriteCallback(uv_write_t* req, int status);
		static void CloseCallback(uv_handle_t* client);
		static void WorkCallback(uv_work_t* work);
		static void AfterWorkCallback(uv_work_t* work, int status);
	public:
		UvTcpServer(shared_ptr<TcpServer> server);
		~UvTcpServer();
		int Start(const string& ip, int port);
		int Stop();
		int Write(shared_ptr<ClientSession> session, shared_ptr<write_buffer_t> wrbuf);
		int Close(shared_ptr<ClientSession> session);
		int Close(const string& id);
		int AsyncExec(shared_ptr<Task> task);
	protected:
		shared_ptr<TcpServer> GetServer();
		void DoAccept();
		void DoWrite();
		void WriteClean(const string& id);
		void TaskClean(const string& id);
		void OnTimer();
	private:
		weak_ptr<TcpServer> server_;
		uv_loop_t loop_;
		uv_async_t async_;
		uv_tcp_t listener_;
		SimpleQueue<WriteTask> wrlist_;
		unordered_map<string, shared_ptr<WriteTask>> written_;
		uv_mutex_t task_queue_mutex_;
		unordered_map<string, shared_ptr<AsyncTask>> async_task_list_;
		uv_timer_t timer_;
	};
} // namespace hlp

#endif // SERVER_TCP_UV_TCP_SERVER_H_

