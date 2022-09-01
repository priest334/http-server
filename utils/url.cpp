#include "url.h"

#include <sstream>
#include "third_party/http_parser/http_parser.h"
#include "string_helper.h"


using std::ostringstream;
namespace simple {
	namespace {
		const int ishexchar[] = {
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
			0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		};
		const int hexval[] = {
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,
			0,0xa,0xb,0xc,0xd,0xe,0xf,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0xa,0xb,0xc,0xd,0xe,0xf,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		};
		const char* urlchar[] = {
			"%00", "%01", "%02", "%03", "%04", "%05", "%06", "%07", "%08", "%09", "%0A", "%0B", "%0C", "%0D", "%0E", "%0F",
			"%10", "%11", "%12", "%13", "%14", "%15", "%16", "%17", "%18", "%19", "%1A", "%1B", "%1C", "%1D", "%1E", "%1F",
			"%20", "!", "%22", "%23", "%24", "%25", "%26", "%27", "(", ")", "*", "%2B", "%2C", "%2D", ".", "%2F",
			"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "%3A", "%3B", "%3C", "%3D", "%3E", "%3F",
			"%40", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
			"P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "%5B", "%5C", "%5D", "%5E", "_",
			"%60", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
			"p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "%7B", "%7C", "%7D", "~", "%7F",
			"%80", "%81", "%82", "%83", "%84", "%85", "%86", "%87", "%88", "%89", "%8A", "%8B", "%8C", "%8D", "%8E", "%8F",
			"%90", "%91", "%92", "%93", "%94", "%95", "%96", "%97", "%98", "%99", "%9A", "%9B", "%9C", "%9D", "%9E", "%9F",
			"%A0", "%A1", "%A2", "%A3", "%A4", "%A5", "%A6", "%A7", "%A8", "%A9", "%AA", "%AB", "%AC", "%AD", "%AE", "%AF",
			"%B0", "%B1", "%B2", "%B3", "%B4", "%B5", "%B6", "%B7", "%B8", "%B9", "%BA", "%BB", "%BC", "%BD", "%BE", "%BF",
			"%C0", "%C1", "%C2", "%C3", "%C4", "%C5", "%C6", "%C7", "%C8", "%C9", "%CA", "%CB", "%CC", "%CD", "%CE", "%CF",
			"%D0", "%D1", "%D2", "%D3", "%D4", "%D5", "%D6", "%D7", "%D8", "%D9", "%DA", "%DB", "%DC", "%DD", "%DE", "%DF",
			"%E0", "%E1", "%E2", "%E3", "%E4", "%E5", "%E6", "%E7", "%E8", "%E9", "%EA", "%EB", "%EC", "%ED", "%EE", "%EF",
			"%F0", "%F1", "%F2", "%F3", "%F4", "%F5", "%F6", "%F7", "%F8", "%F9", "%FA", "%FB", "%FC", "%FD", "%FE", "%FF"
		};
	}

	string Url::Encode(const string& value) {
		string retval;
		size_t len = value.length();
		for (size_t i = 0; i < len; i++) {
			retval.append(urlchar[(unsigned char)value[i]]);
		}
		return retval;
	}

	string Url::Decode(const string& value) {
		string retval;
		size_t i = 0, j = 0, len = value.length();
		for (; i < len; j++) {
			char c = value[i];
			if (c == '%' && (i + 2) < len && 1 == ishexchar[value[i + 1]] && 1 == ishexchar[value[i + 2]]) {
				char ch = ((hexval[value[i + 1]] << 4) & 0xf0) | (hexval[value[i + 2]] & 0x0f);
				retval.append(1, ch);
				i += 3;
			} else {
				retval.append(1, c);
				i++;
			}
		}
		return retval;
	}

	Url::Url() {
	}

	Url::Url(const string& url) {
		Parse(url);
	}

	void Url::Parse(const string& url) {
		http_parser_url parser;
		http_parser_url_init(&parser);
		http_parser_parse_url(url.c_str(), url.length(), false, &parser);
		schema_ = url.substr(parser.field_data[UF_SCHEMA].off, parser.field_data[UF_SCHEMA].len);
		user_info_ = url.substr(parser.field_data[UF_USERINFO].off, parser.field_data[UF_USERINFO].len);
		host_ = url.substr(parser.field_data[UF_HOST].off, parser.field_data[UF_HOST].len);
		port_ = url.substr(parser.field_data[UF_PORT].off, parser.field_data[UF_PORT].len);
		path_ = url.substr(parser.field_data[UF_PATH].off, parser.field_data[UF_PATH].len);
		fragment_ = url.substr(parser.field_data[UF_FRAGMENT].off, parser.field_data[UF_FRAGMENT].len);
		if (parser.field_data[UF_QUERY].len > 0) {
			size_t qlen = parser.field_data[UF_QUERY].len;
			const char* qs = &url.at(parser.field_data[UF_QUERY].off);
			bool field = true;
			string key;
			for (size_t i = 0, start = 0, len = 0; i < qlen; i++) {
				if (field) {
					if (*(qs + i) == '&')
						break;
					if (*(qs + i) == '=') {
						key.assign(qs + start, len);
						field = !field;
						len = 0;
						start = i + 1;
					} else {
						len++;
					}
				} else {
					if (*(qs + i) == '&') {
						queries_[key] = Decode(string(qs + start, len));
						field = !field;
						len = 0;
						start = i + 1;
					} else {
						len++;
						if ((i + 1) == qlen) {
							queries_[key] = Decode(string(qs + start, len));
							break;
						}
					}
				}
			}
		}
	}

	string Url::Make() const {
		ostringstream oss;
		if (!schema_.empty()) {
			oss << schema_ << "://";
		}
		if (!user_info_.empty()) {
			oss << user_info_ << "@";
		}
		if (!host_.empty()) {
			oss << host_;
			if (!port_.empty()) {
				oss << ":" << port_;
			}
		}
		oss << (path_.empty() ? "/" : path_);
		if (!queries_.empty()) {
			bool first_element = true;
			for (const auto& it : queries_) {
				if (first_element) {
					first_element = false;
					oss << "?";
				} else {
					oss << "&";
				}
				oss << it.first << "=" << Encode(it.second);
			}
		}
		if (!fragment_.empty()) {
			oss << "#" << fragment_;
		}
		return oss.str();
	}

	string Url::Schema() const {
		return schema_;
	}

	string Url::UserInfo() const {
		return user_info_;
	}

	string Url::Host() const {
		return host_;
	}

	string Url::Port() const {
		return port_;
	}

	string Url::Path() const {
		return path_;
	}

	string Url::Fragment() const {
		return fragment_;
	}

	string Url::Query(const string& key) const {
		if (key.empty()) {
			return key;
		}
		auto it = queries_.find(key);
		if (it != queries_.end()) {
			return it->second;
		}
		return string();
	}

	const unordered_map<string, string>& Url::Queries() const {
		return queries_;
	}

	Url& Url::SetScehma(const string& schema) {
		schema_ = schema;
		return *this;
	}

	Url& Url::SetHost(const string& host) {
		host_ = host;
		return *this;
	}

	Url& Url::SetUserInfo(const string& user_info) {
		user_info_ = user_info;
		return *this;
	}

	Url& Url::SetPort(const string& port) {
		port_ = port;
		return *this;
	}

	Url& Url::SetPath(const string& path) {
		path_ = path;
		return *this;
	}

	Url& Url::SetFragment(const string& fragment) {
		fragment_ = fragment;
		return *this;
	}

	Url& Url::SetQuery(const string& key, const string& value) {
		if (!key.empty()) {
			queries_[key] = value;
		}
		return *this;
	}

	Url& Url::SetQueries(const unordered_map<string, string>& queries) {
		for (auto& it : queries) {
			queries_[it.first] = it.second;
		}
		return *this;
	}
} // namespace simple

