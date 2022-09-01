#pragma once
#ifndef SERVER_HTTP_SERVER_H_
#define SERVER_HTTP_SERVER_H_

#include <memory>
#include <string>
#include "tcp_server.h"


using std::string;
using std::weak_ptr;
namespace simple {
	class HttpApplication;
	class HttpRequest;
	class HttpResponse;
	class HttpServer
		: public IoEventHandler,
		public std::enable_shared_from_this<HttpServer> {
	public:
		HttpServer(shared_ptr<HttpApplication> application);
		shared_ptr<HttpResponse> CreateResponse(shared_ptr<ClientSession> session, int status_code);
		int OnCreate(shared_ptr<ClientSession> session) override;
		int OnRead(shared_ptr<ClientSession> session, shared_ptr<read_buffer_t> rdbuf, int size) override;
		int OnWrite(shared_ptr<ClientSession> session, shared_ptr<write_buffer_t> wrbuf, int status) override;
		int OnClose(shared_ptr<ClientSession> session) override;
		int OnTimeout(shared_ptr<ClientSession> session, int how) override;
		int Start(const string& ip, int port);
		void Dispatch(shared_ptr<ClientSession> session, shared_ptr<HttpRequest> request);
	protected:
		shared_ptr<simple::TcpServer> server_;
		weak_ptr<HttpApplication> application_;
	};
} // namespace simple

#endif // SERVER_HTTP_SERVER_H_

