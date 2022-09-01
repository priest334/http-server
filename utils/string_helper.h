#pragma once
#ifndef UTILS_STRING_HELPER_H_
#define UTILS_STRING_HELPER_H_

#include <cstdarg>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>


using std::map;
using std::string;
using std::unordered_map;
using std::vector;
using std::ostringstream;
namespace simple {
	class String {
	public:
		static String Digits;
		static String CapitalLetters;
		static String SmallLetters;
		static String Letters;
		static String Normal;
		static String Printable;
		static String WhiteSpace;
		static String Random(size_t length, const String& chars);
		static String Random(size_t length);

		String();
		String(size_t size);
		String(const char* str);
		String(const char* str, size_t length);
		String(const string& str);
		String(const String& str);
		String& Vsnprintf(size_t size, const char* fmt, va_list args);
		String& Snprintf(size_t size, const char* fmt, ...);
		String& Sprintf(const char* fmt, ...);
		String& Format(size_t size, const char* fmt, ...);
		String& Format(const char* fmt, ...);
		String& Assign(const char* str);
		String& Assign(const string& str);
		String& Assign(const String& str);
		String& Append(const char* str);
		String& Append(const string& str);
		String& Append(const String& str);
		template<class _ElementType>
		String Join(const vector<_ElementType>& values) const {
			if (values.empty())
				return String();
			ostringstream oss;
			bool first_element = true;
			for (auto& it : values) {
				if (first_element) {
					first_element = false;
				} else {
					oss << value_;
				}
				oss << it;
			}
			return String(oss.str());
		}
		template<class _Map>
		String Join(const _Map& values, const string& assign_key) const {
			if (values.empty())
				return String();
			std::ostringstream oss;
			auto it = values.begin();
			for (;;) {
				oss << it->first << assign_key << it->second;
				if (++it == values.end())
					break;
				oss << value_;
			}
			return String(oss.str());
		}
		String Center(size_t length, char fill = ' ') const;
		String StripLeft(const String& chars) const;
		String StripLeft() const;
		String StripRight(const String& chars) const;
		String StripRight() const;
		String Strip(const String& chars) const;
		String Strip() const;
		String SubStr(int start, int end = -1) const;
		String RemovePrefix(const String& str) const;
		String RemoveSuffix(const String& str) const;
		String Replace(const String& old, const String& str) const;
		vector<String> Split(const String& sep, size_t maxsplit = -1) const;
		unordered_map<String, String> Split(const String& sep, const string& assign_key) const;
		String CamelToUnderscore() const;
		String UnderscoreToCamel(bool little = true) const;
		String Translate(const unordered_map<char, char>& table) const;
		String ToUpper() const;
		String ToLower() const;
		
		bool IsDigit() const;
		bool StartsWith(const String& prefix) const;
		bool EndsWith(const String& suffix) const;
		int Find(const String& sub) const;
		int ToInt() const;
		int64_t ToInt64() const;
		String& resize(size_t new_size, const char fill = (char)0);
		char& operator[](int index);
		char operator[](int index) const;
		string str() const;
		char const * c_str() const;
		size_t length() const;
		size_t size() const;
		operator int() const;
		operator char const* () const;
		operator string const& () const;

		String& operator=(const char* str);
		String& operator=(const string& str);
		String& operator=(const String& str);
		String& operator+=(const char* str);
		String& operator+=(const string& str);
		String& operator+=(const String& str);
		friend bool operator<(const char* left, const String& right);
		friend bool operator<(const String& left, const char* right);
		friend bool operator<(const string& left, const String& right);
		friend bool operator<(const String& left, const string& right);
		friend bool operator<(const String& left, const String& right);
		friend bool operator==(const char* left, const String& right);
		friend bool operator==(const String& left, const char* right);
		friend bool operator==(const string& left, const String& right);
		friend bool operator==(const String& left, const string& right);
		friend bool operator==(const String& left, const String& right);
		friend bool operator>(const char* left, const String& right);
		friend bool operator>(const String& left, const char* right);
		friend bool operator>(const string& left, const String& right);
		friend bool operator>(const String& left, const string& right);
		friend bool operator>(const String& left, const String& right);
		friend bool operator!=(const char* left, const String& right);
		friend bool operator!=(const String& left, const char* right);
		friend bool operator!=(const string& left, const String& right);
		friend bool operator!=(const String& left, const string& right);
		friend bool operator!=(const String& left, const String& right);
		friend bool operator<=(const char* left, const String& right);
		friend bool operator<=(const String& left, const char* right);
		friend bool operator<=(const string& left, const String& right);
		friend bool operator<=(const String& left, const string& right);
		friend bool operator<=(const String& left, const String& right);
		friend bool operator>=(const char* left, const String& right);
		friend bool operator>=(const String& left, const char* right);
		friend bool operator>=(const string& left, const String& right);
		friend bool operator>=(const String& left, const string& right);
		friend bool operator>=(const String& left, const String& right);
		friend std::ostream& operator<<(std::ostream& stream, const String& str);
	private:
		string value_;
	};
	String operator+(String left, const char* right);
	String operator+(String left, const string& right);
	String operator+(String left, const String& right);
	String operator+(const char* left, const String& right);
	String operator+(const string& left, const String& right);
} // namespace simple

template<>
struct std::hash<simple::String> {
	std::size_t operator()(const simple::String& s) const noexcept {
		return std::hash<std::string>{}(s.str());
	}
};

#endif // UTILS_STRING_HELPER_H_

