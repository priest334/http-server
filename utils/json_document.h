#pragma once
#ifndef UTILS_JSON_DOCUMENT_H_
#define UTILS_JSON_DOCUMENT_H_

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>


using std::shared_ptr;
using std::string;
using std::vector;
using std::unordered_map;
namespace Json {
	class Value;
}
namespace simple {
	using JsonValue = Json::Value;
	typedef JsonValue* PJsonValue;
	class JsonDocument {
		static bool AsBool(PJsonValue value, bool default_value);
		static int64_t AsInt64(PJsonValue value, int64_t default_value);
		static float AsFloat(PJsonValue value, float default_value);
		static double AsDouble(PJsonValue value, double default_value);
		static string AsString(PJsonValue value);
		static string AsString(PJsonValue value, const char* default_value);
		static string AsString(PJsonValue value, const string& default_value);
	public:
		JsonDocument();
		bool Parse(const string& doc);
		string Print(bool pretty = true) const;
		string Print(const string& key, bool pretty = true, bool dot_notation_enabled = true) const;

		JsonDocument& SetNull(const string& key, bool dot_notation_enabled = true);
		JsonDocument& SetBool(const string& key, bool value, bool dot_notation_enabled = true);
		JsonDocument& SetInt64(const string& key, int64_t value, bool dot_notation_enabled = true);
		JsonDocument& SetFloat(const string& key, float value, bool dot_notation_enabled = true);
		JsonDocument& SetDouble(const string& key, double value, bool dot_notation_enabled = true);
		template<class NumericType>
		JsonDocument& Set(const string& key, NumericType value, bool dot_notation_enabled = true) {
			SetInt64(key, static_cast<int64_t>(value), dot_notation_enabled);
			return *this;
		}
		JsonDocument& Set(const string& key, PJsonValue value, bool dot_notation_enabled = true);
		JsonDocument& Set(const string& key, const char* value, bool dot_notation_enabled = true);
		JsonDocument& Set(const string& key, const string& value, bool dot_notation_enabled = true);
		JsonDocument& Set(const string& key, shared_ptr<JsonDocument> value, bool dot_notation_enabled = true);
		template <class NumericType>
		JsonDocument& SetNumeric(const string& key, const string& value, bool dot_notation_enabled = true) {
			if (typeid(NumericType) == typeid(float)) {
				float float_value = std::strtof(value.c_str(), nullptr);
				SetFloat(key, float_value, dot_notation_enabled);
			} else if (typeid(NumericType) == typeid(double)) {
				double double_value = std::strtod(value.c_str(), nullptr);
				SetDouble(key, double_value, dot_notation_enabled);
			} else {
				int64_t int64_value = std::strtoll(value.c_str(), nullptr, 10);
				SetInt64(key, int64_value, dot_notation_enabled);
			}
			return *this;
		}
		JsonDocument& AppendBool(const string& key, bool value, bool dot_notation_enabled = true);
		JsonDocument& AppendInt64(const string& key, int64_t value, bool dot_notation_enabled = true);
		JsonDocument& AppendFloat(const string& key, float value, bool dot_notation_enabled = true);
		JsonDocument& AppendDouble(const string& key, double value, bool dot_notation_enabled = true);
		template<class NumericType>
		JsonDocument& Append(const string& key, NumericType value, bool dot_notation_enabled = true) {
			AppendInt64(key, static_cast<int64_t>(value), dot_notation_enabled);
			return *this;
		}
		JsonDocument& Append(const string& key, PJsonValue value, bool dot_notation_enabled = true);
		JsonDocument& Append(const string& key, const char* value, bool dot_notation_enabled = true);
		JsonDocument& Append(const string& key, const string& value, bool dot_notation_enabled = true);
		JsonDocument& Append(const string& key, shared_ptr<JsonDocument> value, bool dot_notation_enabled = true);
		template <class NumericType>
		JsonDocument& AppendNumeric(const string& key, const string& value, bool dot_notation_enabled = true) {
			if (typeid(NumericType) == typeid(float)) {
				float float_value = std::strtof(value.c_str(), nullptr);
				AppendFloat(key, float_value, dot_notation_enabled);
			} else if (typeid(NumericType) == typeid(double)) {
				double double_value = std::strtod(value.c_str(), nullptr);
				AppendDouble(key, double_value, dot_notation_enabled);
			} else {
				int64_t int64_value = std::strtoll(value.c_str(), nullptr, 10);
				AppendInt64(key, int64_value, dot_notation_enabled);
			}
			return *this;
		}
		JsonDocument& SetArray(const string& key, bool dot_notation_enabled = true);
		template <class ArrayElementType>
		JsonDocument& AppendArray(const string& key, const vector<ArrayElementType>& values, bool dot_notation_enabled = true) {
			for (auto it : values) {
				Append(key, it, dot_notation_enabled);
			}
			return *this;
		}
		template <class ArrayElementType>
		JsonDocument& SetArray(const string& key, const vector<ArrayElementType>& values, bool dot_notation_enabled = true) {
			if (values.empty()) {
				SetArray(key, dot_notation_enabled);
			} else {
				AppendArray(key, values, dot_notation_enabled);
			}
			return *this;
		}

		bool HasKey(const string& key, bool dot_notation_enabled = true) const;
		bool IsEmpty() const;
		bool IsNull(const string& key, bool dot_notation_enabled = true) const;
		bool IsNumeric(const string& key, bool dot_notation_enabled = true) const;
		bool IsString(const string& key, bool dot_notation_enabled = true) const;
		bool IsArray(const string& key, bool dot_notation_enabled = true) const;
		bool IsDocument(const string& key, bool dot_notation_enabled = true) const;
		int ArraySize(const string& key, bool dot_notation_enabled = true) const;
		bool AsBool(const string& key, bool default_value = false, bool dot_notation_enabled = true) const;
		int64_t AsInt64(const string& key, int64_t default_value = 0, bool dot_notation_enabled = true) const;
		int AsInt(const string& key, int default_value = 0, bool dot_notation_enabled = true) const;
		float AsFloat(const string& key, float default_value = 0.0f, bool dot_notation_enabled = true) const;
		double AsDouble(const string& key, double default_value = 0.0, bool dot_notation_enabled = true) const;
		string AsString(const string& key, bool dot_notation_enabled = true) const;
		string AsString(const string& key, const char* default_value, bool dot_notation_enabled = true) const;
		string AsString(const string& key, const string& default_value, bool dot_notation_enabled = true) const;
		shared_ptr<JsonDocument> AsDocument(const string& key, bool dot_notation_enabled = true) const;
		vector<bool> AsBoolArray(const string& key, bool dot_notation_enabled = true) const;
		vector<int64_t> AsInt64Array(const string& key, bool dot_notation_enabled = true) const;
		vector<float> AsFloatArray(const string& key, bool dot_notation_enabled = true) const;
		vector<double> AsDoubleArray(const string& key, bool dot_notation_enabled = true) const;
		vector<string> AsStringArray(const string& key, bool dot_notation_enabled = true) const;
		vector<shared_ptr<JsonDocument>> AsDocumentArray(const string& key, bool dot_notation_enabled = true) const;

		JsonDocument& Merge(shared_ptr<JsonDocument> src, bool dot_notation_enabled = true);
		JsonDocument& Copy(shared_ptr<JsonDocument> src, const string& key, const string& new_key, bool create_key_if_not_exists = false, bool dot_notation_enabled = true);
		JsonDocument& Copy(shared_ptr<JsonDocument> src, const vector<string>& keys, const vector<string>& new_keys, bool create_key_if_not_exists = false, bool dot_notation_enabled = true);
		JsonDocument& Merge(shared_ptr<JsonDocument> src, const string& key, bool create_key_if_not_exists = false, bool dot_notation_enabled = true);
		JsonDocument& Merge(shared_ptr<JsonDocument> src, const vector<string>& keys, bool create_key_if_not_exists = false, bool dot_notation_enabled = true);
		JsonDocument& Move(const string& old_key, const string& new_key, bool dot_notation_enabled = true);
		shared_ptr<JsonDocument> Remove(const string& key, bool dot_notation_enabled = true);
		string RemoveAsString(const string& key, bool dot_notation_enabled = true);
		JsonDocument& RenameKey(const string& old_key, const string& new_key, int max_depth = -1);
		JsonDocument& RenameKeys(string (*rename_handler)(const string&), int max_depth = -1);
		shared_ptr<JsonDocument> DupKey(const string& key, bool dot_notation_enabled = true) const;
		shared_ptr<JsonDocument> DupKeys(const vector<string>& keys, bool dot_notation_enabled = true) const;
		shared_ptr<JsonDocument> DotNotation() const;
		vector<string> Keys() const;
		vector<string> Values() const;
	private:
		shared_ptr<Json::Value> root_;
	};
} // namespace simple

#endif // UTILS_JSON_DOCUMENT_H_

