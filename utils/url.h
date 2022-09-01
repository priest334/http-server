#pragma once
#ifndef UTILS_URL_H_
#define UTILS_URL_H_

#include <string>
#include <unordered_map>


using std::string;
using std::unordered_map;
namespace simple {
	class Url {
	public:
		static string Encode(const string& value);
		static string Decode(const string& value);
		Url();
		Url(const string& url);
		void Parse(const string& url);
		string Make() const;
		string Schema() const;
		string UserInfo() const;
		string Host() const;
		string Port() const;
		string Path() const;
		string Fragment() const;
		string Query(const string& key) const;
		const unordered_map<string, string>& Queries() const;
		Url& SetScehma(const string& schema);
		Url& SetHost(const string& host);
		Url& SetUserInfo(const string& user_info);
		Url& SetPort(const string& port);
		Url& SetPath(const string& path);
		Url& SetFragment(const string& fragment);
		Url& SetQuery(const string& key, const string& value);
		Url& SetQueries(const unordered_map<string, string>& queries);
	private:
		string schema_;
		string user_info_;
		string host_;
		string port_;
		string path_;
		string fragment_;
		unordered_map<string, string> queries_;
	};
} // namespace simple

#endif // UTILS_URL_H_

