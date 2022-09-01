#pragma once
#ifndef SERVER_HTTP_REQUEST_H_
#define SERVER_HTTP_REQUEST_H_

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>


using std::shared_ptr;
using std::string;
using std::unordered_map;
using std::weak_ptr;
namespace simple {
	class Url;
	class JsonDocument;
	class HttpParser;
	class ClientSession;
	class HttpRequest {
		typedef unordered_map<string, string> PathArgs;
		typedef unordered_map<string, string> Headers;
		typedef unordered_map<string, string> Queries;
		typedef unordered_map<string, string> Cookies;
		typedef unordered_map<string, string> FormData;
		friend class HttpParser;
	public:
		HttpRequest(shared_ptr<ClientSession> session);
		void SetMethod(const string& method);
		void SetUrl(const string& url);
		void SetHeader(const string& key, const string& value);
		void SetHeaderDone();
		void SetBody(const string& body);
		void SetDone();
		bool IsDone() const;
		string Method() const;
		string Url() const;
		string Path() const;
		string Path(const string& name) const;
		string Query(const string& key) const;
		string Header(const string& key) const;
		string Body() const;
		shared_ptr<JsonDocument> Json() const;
		void SetJson(shared_ptr<JsonDocument> json);
		string Json(const string& key) const;
		string Form(const string& key) const;
		string Cookie(const string& key) const;
		string ContentType() const;
		size_t ContentLength() const;
		string Param(const string& key) const;
		PathArgs& PathArgList();
		const PathArgs& PathArgList() const;
		void SetUserContext(shared_ptr<void> user_context);
		shared_ptr<void> UserContext();
		shared_ptr<void> UserContext() const;
	private:
		weak_ptr<ClientSession> session_;
		shared_ptr<HttpParser> parser_;
		string method_;
		string path_;
		string body_;
		bool done_;
		shared_ptr<simple::Url> url_;
		shared_ptr<JsonDocument> json_;
		PathArgs path_args_;
		Headers headers_;
		Cookies cookies_;
		FormData form_;
		shared_ptr<void> user_context_;
	};
} // namespace simple

#endif // SERVER_HTTP_REQUEST_H_

