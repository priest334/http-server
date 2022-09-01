#include "http_server.h"

#include <sstream>
#include "utils/task.h"
#include "buffer.h"
#include "http/application.h"
#include "http/parser.h"
#include "http/request.h"
#include "http/response.h"


using std::ostringstream;
namespace simple {
	class HttpContext {
	public:
		HttpContext(shared_ptr<ClientSession> session)
			: session_(session) {
			parser_ = std::make_shared<HttpParser>();
		}

		shared_ptr<HttpRequest> Read(const char* data, size_t size) {
			if (!request_) {
				request_ = std::make_shared<HttpRequest>(session_);
			}
			parser_->Parse(request_, data, size);
			if (request_->IsDone()) {
				return request_;
			}
			return nullptr;
		}
	private:
		shared_ptr<ClientSession> session_;
		shared_ptr<HttpParser> parser_;
		shared_ptr<HttpRequest> request_;
	};

	class HandleRequestTask 
		: public Task {
	public:
		HandleRequestTask(shared_ptr<HttpServer> server, shared_ptr<ClientSession> session, shared_ptr<HttpRequest> request)
			: server_(server), session_(session), request_(request) {
		}

		virtual void Run() override {
			server_->Dispatch(session_, request_);
		}

		virtual void Done() override {
		}

	private:
		shared_ptr<HttpServer> server_;
		shared_ptr<ClientSession> session_;
		shared_ptr<HttpRequest> request_;
	};

	HttpServer::HttpServer(shared_ptr<HttpApplication> application)
		: application_(application) {
	}

	shared_ptr<HttpResponse> HttpServer::CreateResponse(shared_ptr<ClientSession> session, int status_code) {
		auto application = application_.lock();
		if (!application) {
			exit(-1);
		}
		auto response = std::make_shared<HttpResponse>(session);
		response->SetStatusCode(status_code);
		string server_name = application->Name();
		response->SetHeader("Server", server_name);
		return response;
	}

	int HttpServer::OnCreate(shared_ptr<ClientSession> session) {
		auto context = std::make_shared<HttpContext>(session);
		session->SetTimeout(30, 0x1);
		session->SetUserContext(context);
		return 0;
	}

	int HttpServer::OnRead(shared_ptr<ClientSession> session, shared_ptr<read_buffer_t> rdbuf, int size) {
		auto context = std::static_pointer_cast<HttpContext>(session->UserContext());
		auto request = context->Read(rdbuf->data(), (size_t)size);
		if (request) {
			session->SetTimeout(-1, 0x1);
			session->SetTimeout(60, 0x2);
			server_->AsyncExec(std::make_shared<HandleRequestTask>(shared_from_this(), session, request));
		}
		return 0;
	}

	int HttpServer::OnWrite(shared_ptr<ClientSession> session, shared_ptr<write_buffer_t> wrbuf, int status) {
		return 0;
	}

	int HttpServer::OnClose(shared_ptr<ClientSession> session) {
		return 0;
	}

	int HttpServer::OnTimeout(shared_ptr<ClientSession> session, int how) {
		if (how == 0x1) {
			auto response = CreateResponse(session, 408);
			response->Send();
		}
		if (how == 0x2) {
			auto response = CreateResponse(session, 504);
			response->Send();
		}
		session->Close();
		return 0;
	}

	int HttpServer::Start(const string& ip, int port) {
		server_ = std::make_shared<simple::TcpServer>(shared_from_this());
		return server_->Start(ip, port);
	}

	void HttpServer::Dispatch(shared_ptr<ClientSession> session, shared_ptr<HttpRequest> request) {
		auto application = application_.lock();
		if (!application) {
			exit(-1);
		}
		auto response = CreateResponse(session, 404);
		response->SetHeader("Request", session->Id());
		application->Process(request, response);
		response->Send();
		session->Close();
	}
} // namespace simple


