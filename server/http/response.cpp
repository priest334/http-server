#include "response.h"

#include <ctime>
#include <sstream>
#include "utils/logger.h"
#include "utils/json_document.h"
#include "utils/third_party/http_parser/http_parser.h"
#include "buffer.h"
#include "client_session.h"


using std::ostringstream;
namespace simple {
	HttpResponse::HttpResponse(shared_ptr<ClientSession> session)
		: session_(session),
		status_code_(HTTP_STATUS_NOT_FOUND) {
	}

	void HttpResponse::SetStatusCode(int code) {
		status_code_ = code;
	}

	void HttpResponse::SetHeader(const string& key, const string& value) {
		auto it = headers_.find(key);
		if (it != headers_.end()) {
			headers_.erase(key);
		}
		headers_.insert(std::make_pair(key, value));
	}

	void HttpResponse::SetPayload(const string& payload) {
		std::ostringstream oss;
		oss << payload.length();
		SetHeader("Content-Length", oss.str());
		payload_ = payload;
	}

	void HttpResponse::SetPayload(shared_ptr<JsonDocument> payload) {
		json_ = payload;
		SetHeader("Content-Type", "application/json;charset=utf8");
		SetPayload(payload->Print(false));
	}

	void HttpResponse::SetCookie(const string& cookie) {
		headers_.insert(std::make_pair("Set-Cookie", cookie));
	}

	void HttpResponse::SetCookie(const string& name, const string& value, bool http_only/* = false*/) {
		std::ostringstream oss;
		oss << name << "=" << value;
		if (http_only) {
			oss << "; HttpOnly";
		}
		SetCookie(oss.str());
	}

	void HttpResponse::Redirect(const string& url) {
		SetHeader("Location", url);
		SetStatusCode(HTTP_STATUS_FOUND);
	}

	int HttpResponse::StatusCode() const {
		return status_code_;
	}

	string HttpResponse::ContentType() const {
		auto it = headers_.find("Content-Type");
		if (it != headers_.end()) {
			return it->second;
		}
		return string();
	}

	string HttpResponse::Payload() const {
		return payload_;
	}

	shared_ptr<JsonDocument> HttpResponse::JsonPayload() const {
		return json_;
	}

	std::ostream& operator<<(std::ostream& stream, const HttpResponse& response) {
		stream << "HTTP/1.1 " << response.status_code_ << " " << http_status_str(http_status(response.status_code_)) << "\r\n";
		auto it = response.headers_.begin();
		for (; it != response.headers_.end(); ++it) {
			stream << it->first << ": " << it->second << "\r\n";
		}
		stream << "\r\n";
		stream << response.payload_;
		return stream;
	}

	void HttpResponse::Send() {
		ostringstream oss;
		oss << "HTTP/1.1 " << status_code_ << " " << http_status_str(http_status(status_code_)) << "\r\n";
		auto it = headers_.begin();
		for (; it != headers_.end(); ++it) {
			oss << it->first << ": " << it->second << "\r\n";
		}
		oss << "\r\n";
		oss << payload_;
		string buffer = oss.str();
		auto data = std::make_shared<write_buffer_t>(buffer.begin(), buffer.end());
		auto session = session_.lock();
		if (!session) {
			logger::Error() << "send response failed: session lost" << std::endl;
			return;
		}
		session->Write(data);
	}
} // namespace simple

