#include "request.h"

#include <iostream>
#include <sstream>
#include "parser.h"
#include "utils/json_document.h"
#include "utils/stl_helper.h"
#include "utils/string_helper.h"
#include "utils/url.h"


namespace simple {
	const char* const content_type_form = "application/x-www-form-urlencoded";
	const char* const content_type_json = "application/json";
	const char* const content_type_multipart_form = "multipart/form-data";

	HttpRequest::HttpRequest(shared_ptr<ClientSession> session)
		: session_(session), done_(false) {
	}

	void HttpRequest::SetMethod(const string& method) {
		method_ = method;
	}

	void HttpRequest::SetUrl(const string& url) {
		url_ = std::make_shared<simple::Url>(url);
	}

	void HttpRequest::SetHeader(const string& key, const string& value) {
		headers_[String(key).ToLower().str()] = value;
	}

	void HttpRequest::SetHeaderDone() {
		// 
	}

	void HttpRequest::SetBody(const string& body) {
		body_ += body;
	}

	void HttpRequest::SetDone() {
		done_ = true;
		string content_type = ContentType();
		if (content_type.find(content_type_json) != string::npos) {
			json_ = std::make_shared<JsonDocument>();
			json_->Parse(body_);
		} else if (content_type.find(content_type_form) != string::npos) {
			auto kvp = String(body_).Split("&", "=");
			for (const auto& it : kvp) {
				form_[it.first] = Url::Decode(it.second);
			}
		}
	}

	bool HttpRequest::IsDone() const {
		return done_;
	}

	string HttpRequest::Method() const {
		return method_;
	}

	string HttpRequest::Url() const {
		return url_->Make();
	}

	string HttpRequest::Path() const {
		return url_->Path();
	}

	string HttpRequest::Path(const string& name) const {
		return MapValue(path_args_, name);
	}

	string HttpRequest::Query(const string& key) const {
		return url_->Query(key);
	}

	string HttpRequest::Header(const string& key) const {
		return MapValue(headers_, String(key).ToLower().str());
	}

	string HttpRequest::Body() const {
		return body_;
	}

	shared_ptr<JsonDocument> HttpRequest::Json() const {
		return json_;
	}

	void HttpRequest::SetJson(shared_ptr<JsonDocument> json) {
		json_ = json;
	}

	string HttpRequest::Json(const string& key) const {
		if (json_ && !key.empty()) {
			return json_->AsString(key);
		}
		return string();
	}

	string HttpRequest::Form(const string& key) const {
		return MapValue(form_, key);
	}


	string HttpRequest::Cookie(const string& key) const {
		return MapValue(cookies_, key);
	}

	string HttpRequest::ContentType() const {
		return Header("Content-Type");
	}

	size_t HttpRequest::ContentLength() const {
		string content_length = Header("Content-Length");
		return std::strtoul(content_length.c_str(), nullptr, 10);
	}
		
	string HttpRequest::Param(const string& key) const {
		const int kSearchQuery = 0;
		const int kSearchPayload = 1;
		const int kSearchHeader = 2;
		const int kSearchDone = 3;
		int search_index = kSearchQuery;
		string value;
		do {
			switch (search_index) {
			case kSearchQuery: {
				value = Query(key);
				break;
			}
			case kSearchPayload: {
				string content_type = ContentType();
				if (content_type == content_type_json) {
					value = Json(key);
				} else if (content_type == content_type_form) {
					value = Form(key);
				}
				break;
			}
			case kSearchHeader: {
				value = Header(key);
				break;
			}
			default:
				search_index = kSearchDone;
			}
			if (!value.empty()) {
				search_index = kSearchDone;
				break;
			}
		} while (search_index++ < kSearchDone);
		return value;
	}

	HttpRequest::PathArgs& HttpRequest::PathArgList() {
		return path_args_;
	}

	const HttpRequest::PathArgs& HttpRequest::PathArgList() const {
		return path_args_;
	}

	void HttpRequest::SetUserContext(shared_ptr<void> user_context) {
		user_context_ = user_context;
	}

	shared_ptr<void> HttpRequest::UserContext() {
		return user_context_;
	}

	shared_ptr<void> HttpRequest::UserContext() const {
		return user_context_;
	}
} // namespace simple

