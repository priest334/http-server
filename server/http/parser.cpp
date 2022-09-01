#include "parser.h"

#include <cstdlib>
#include "request.h"
#include "utils/third_party/http_parser/http_parser.h"
#include "utils/url.h"


namespace simple {
	class HttpParserContext {
		friend class HttpParser;
	public:
		struct http_parser_settings* settings() {
			return &settings_;
		}

		struct http_parser* parser() {
			return &parser_;
		}
	private:
		struct http_parser_settings settings_;
		struct http_parser parser_;
	};
	namespace {
		shared_ptr<HttpRequest> get_current_request(http_parser* parser) {
			HttpParser* p = static_cast<HttpParser*>(parser->data);
			return p->CurrentRequest();
		}

		int on_url(http_parser* parser, const char* at, size_t length) {
			auto request = get_current_request(parser);
			request->SetUrl(string(at, length));
			return 0;
		}

		int on_headers_complete(http_parser* parser) {
			auto request = get_current_request(parser);
			request->SetHeaderDone();
			return 0;
		}

		int on_body(http_parser* parser, const char* at, size_t length) {
			auto request = get_current_request(parser);
			request->SetBody(string(at, length));
			return 0;
		}

		int on_header_field(http_parser* parser, const char* at, size_t length) {
			HttpParser* p = (HttpParser*)parser->data;
			p->SetCurrentKey(string(at, length));
			return 0;
		}

		int on_header_value(http_parser* parser, const char* at, size_t length) {
			HttpParser* p = (HttpParser*)parser->data;
			auto request = get_current_request(parser);
			request->SetHeader(p->GetCurrentKey(), string(at, length));
			return 0;
		}

		int on_message_begin(http_parser* parser) {
			auto request = get_current_request(parser);
			request->SetMethod(http_method_str((http_method)parser->method));
			return 0;
		}

		int on_message_complete(http_parser* parser) {
			auto request = get_current_request(parser);
			request->SetDone();
			return 0;
		}

		int on_chunk_header(http_parser* parser) {
			auto request = get_current_request(parser);
			return 0;
		}

		int on_chunk_complete(http_parser* parser) {
			auto request = get_current_request(parser);
			return 0;
		}
	}

	HttpParser::HttpParser() {
		context_ = std::make_shared<HttpParserContext>();
		http_parser_settings_init(context_->settings());
		context_->settings()->on_message_begin = on_message_begin;
		context_->settings()->on_url = on_url;
		context_->settings()->on_header_field = on_header_field;
		context_->settings()->on_header_value = on_header_value;
		context_->settings()->on_headers_complete = on_headers_complete;
		context_->settings()->on_body = on_body;
		context_->settings()->on_message_complete = on_message_complete;
		context_->settings()->on_chunk_header = on_chunk_header;
		context_->settings()->on_chunk_complete = on_chunk_complete;
		http_parser_init(context_->parser(), HTTP_REQUEST);
		context_->parser()->data = this;
	}

	HttpParser::~HttpParser() {
	}

	int HttpParser::Parse(shared_ptr<HttpRequest> request, const char* data, size_t len) {
		request_ = request;
		http_parser_execute(context_->parser(), context_->settings(), data, len);
		if (request->IsDone()) {
			return 0;
		}
		return -1;
	}

	shared_ptr<HttpRequest> HttpParser::CurrentRequest() const {
		return request_;
	}

	void HttpParser::SetCurrentKey(const string& key) {
		current_key_ = key;
	}

	string HttpParser::GetCurrentKey() const {
		return current_key_;
	}
} // namespace simple

