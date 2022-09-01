#pragma once
#ifndef SERVER_HTTP_RESPONSE_H_
#define SERVER_HTTP_RESPONSE_H_

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>


using std::shared_ptr;
using std::string;
using std::unordered_multimap;
using std::weak_ptr;
namespace simple {
	class ClientSession;
	class JsonDocument;
	class HttpResponse
		: public std::enable_shared_from_this<HttpResponse> {
		typedef std::unordered_multimap<string, string> Headers;
	public:
		HttpResponse(shared_ptr<ClientSession> session);
		void SetStatusCode(int code);
		void SetHeader(const string& key, const string& value);
		void SetPayload(const string& payload);
		void SetPayload(shared_ptr<JsonDocument> payload);
		void SetCookie(const string& cookie);
		void SetCookie(const string& key, const string& value, bool http_only = false);
		void Redirect(const string& url);
		int StatusCode() const;
		string ContentType() const;
		string Payload() const;
		shared_ptr<JsonDocument> JsonPayload() const;
		friend std::ostream& operator<<(std::ostream& stream, const HttpResponse& response);
		void Send();
	private:
		weak_ptr<ClientSession> session_;
		int status_code_;
		Headers headers_;
		string payload_;
		shared_ptr<JsonDocument> json_;
	};
} // namespace simple

#endif // SERVER_HTTP_RESPONSE_H_

