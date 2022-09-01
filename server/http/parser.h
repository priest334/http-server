#pragma once
#ifndef SERVER_HTTP_PARSER_H_
#define SERVER_HTTP_PARSER_H_

#include <memory>
#include <string>


using std::shared_ptr;
using std::string;
namespace simple {
	class HttpParserContext;
	class HttpRequest;
	class HttpParser {
	public:
		HttpParser();
		~HttpParser();
		int Parse(shared_ptr<HttpRequest> request, const char* data, size_t len);
		shared_ptr<HttpRequest> CurrentRequest() const;
		void SetCurrentKey(const string& key);
		string GetCurrentKey() const;
	private:
		shared_ptr<HttpParserContext> context_;
		shared_ptr<HttpRequest> request_;
		string current_key_;
	};
} // namespace simple

#endif // SERVER_HTTP_PARSER_H_

