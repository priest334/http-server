#include "json_document.h"

#include <algorithm>
#include <unordered_map>
#include "string_helper.h"
#include "third_party/jsoncpp/json/json.h"


using std::unordered_map;
namespace simple {
	namespace jsoninternal{
		bool ParseArrayIndex(const string& key, int& index) {
			if (key[0] != '$')
				return false;
			string sindex = key.substr(1);
			for (size_t i = 0; i < sindex.length(); i++) {
				if (sindex[i] < '0' || sindex[i] > '9')
					return false;
			}
			index = std::strtol(sindex.c_str(), nullptr, 10);
			return true;
		}

		bool Get(const Json::Value& parent, const string& key, Json::Value& value, bool dot_notation_enabled = true) {
			if (parent.isObject() && parent.isMember(key)) {
				value = parent[key];
				return true;
			} else if (!dot_notation_enabled) {
				return false;
			}
			int index = -1;
			string::size_type fpos = key.find('.');
			if (fpos == string::npos) {
				if (parent.isArray() && ParseArrayIndex(key, index)) {
					if (!parent.isValidIndex(index))
						return false;
					value = parent[index];
					return true;
				} else if (parent.isObject() && parent.isMember(key)) {
					value = parent[key];
					return true;
				}
				return false;
			} else {
				string pkey = key.substr(0, fpos);
				string subkey = key.substr(fpos + 1);
				if (parent.isArray() && ParseArrayIndex(pkey, index)) {
					if (subkey.empty()) {
						value = parent[index];
						return true;
					} else {
						return Get(parent[index], subkey, value);
					}
				} else if (parent.isObject() && parent.isMember(pkey)) {
					return Get(parent[pkey], subkey, value);
				}
				return false;
			}
		}

		void Set(Json::Value& parent, const string& key, const Json::Value& value, bool dot_notation_enabled = true) {
			string::size_type fpos = key.find('.');
			if (fpos == string::npos || !dot_notation_enabled) {
				parent[key] = value;
			} else {
				string pkey = key.substr(0, fpos);
				if (!parent.isMember(pkey) || !parent[pkey].isObject()) {
					parent[pkey] = Json::Value(Json::objectValue);
				}
				string subkey = key.substr(fpos + 1);
				return Set(parent[pkey], subkey, value);
			}
		}

		void Append(Json::Value& parent, const string& key, const Json::Value& value, bool dot_notation_enabled = true) {
			string::size_type fpos = key.find('.');
			if (fpos == string::npos || !dot_notation_enabled) {
				if (!parent.isMember(key) || !parent[key].isArray()) {
					parent[key] = Json::Value(Json::arrayValue);
				}
				parent[key].append(value);
			} else {
				string pkey = key.substr(0, fpos);
				if (!parent.isMember(pkey) || !parent[pkey].isObject()) {
					parent[pkey] = Json::Value(Json::objectValue);
				}
				string subkey = key.substr(fpos + 1);
				return Append(parent[pkey], subkey, value);
			}
		}

		bool Remove(Json::Value& parent, const string& key, Json::Value& value, bool dot_notation_enabled = true) {
			string::size_type fpos = key.find('.');
			if (fpos == string::npos || !dot_notation_enabled) {
				if (!parent.isMember(key)) {
					value = Json::Value(Json::nullValue);
					return false;
				}
				value = parent.removeMember(key);
				return true;
			} else {
				string pkey = key.substr(0, fpos);
				if (!parent.isMember(pkey)) {
					value = Json::Value(Json::nullValue);
					return false;
				}
				string subkey = key.substr(fpos + 1);
				return Remove(parent[pkey], subkey, value);
			}
		}

		string Write(const Json::Value& value, bool pretty = true) {
			Json::StreamWriterBuilder wbuilder;
			wbuilder["commentStyle"] = "None";
			if (pretty) {
				wbuilder["indentation"] = "\t";
			} else {
				wbuilder["indentation"] = "";
			}
			return Json::writeString(wbuilder, value);
		}

		template<class Fn>
		void RenameKeys(Json::Value& value, Fn rename_handler, int depth, int max_depth) {
			if (max_depth > 0 && depth > max_depth)
				return;
			if (value.isArray()) {
				for (auto& v : value) {
					RenameKeys(v, rename_handler, depth + 1, max_depth);
				}
			} else if (value.isObject()) {
				Json::Value::Members members(value.getMemberNames());
				for (auto it : members) {
					auto& child = value[it];
					RenameKeys(child, rename_handler, depth + 1, max_depth);
					string newkey = rename_handler(it);
					if (newkey != it) {
						value[newkey] = value.removeMember(it);	
					}
				}
			}
		}

		void EnumerateValue(Json::Value* parent, const string& path, const string& key, Json::Value* value, bool (*callback)(Json::Value*, const string&, const string&, Json::Value*, shared_ptr<void>), shared_ptr<void> data) {
			if (value->isArray()) {
				for (auto& element : *value) {
					EnumerateValue(value, path, key, &element, callback, data);
				}
			} else if (value->isObject()) {
				string current_path = key;
				if (!path.empty())
					current_path = path + "." + key;
				Json::Value::Members members(value->getMemberNames());
				for (auto it : members) {
					Json::Value& child = (*value)[it];
					EnumerateValue(value, current_path, it, &child, callback, data);
				}
			} else {
				callback(parent, path, key, value, data);
			}
		}

		void EnumerateRootValue(Json::Value* root, bool (*callback)(Json::Value*, const string&, const string&, Json::Value*, shared_ptr<void>), shared_ptr<void> data) {
			if (!root || !root->isObject())
				return;
			Json::Value::Members members(root->getMemberNames());
			for (auto it : members) {
				Json::Value& child = (*root)[it];
				EnumerateValue(nullptr, "", it, &child, callback, data);
			}
		}
	}

	bool JsonDocument::AsBool(PJsonValue value, bool default_value) {
		if (value->isConvertibleTo(Json::booleanValue)) {
			return value->asBool();
		}
		return default_value;
	}

	int64_t JsonDocument::AsInt64(PJsonValue value, int64_t default_value) {
		if (value->isInt64() || value->isConvertibleTo(Json::intValue) || value->isConvertibleTo(Json::uintValue)) {
			return value->asInt64();
		} else if (value->isString()) {
			return std::strtoll(value->asCString(), nullptr, 10);
		}
		return default_value;
	}

	float JsonDocument::AsFloat(PJsonValue value, float default_value) {
		if (value->isConvertibleTo(Json::realValue)) {
			return value->asFloat();
		} else if (value->isString()) {
			return std::strtof(value->asCString(), nullptr);
		}
		return default_value;
	}

	double JsonDocument::AsDouble(PJsonValue value, double default_value) {
		if (value->isConvertibleTo(Json::realValue)) {
			return value->asDouble();
		} else if (value->isString()) {
			return std::strtod(value->asCString(), nullptr);
		}
		return default_value;
	}

	string JsonDocument::AsString(PJsonValue value) {
		if (value->isConvertibleTo(Json::stringValue)) {
			return value->asString();
		}
		return string();
	}

	string JsonDocument::AsString(PJsonValue value, const char* default_value) {
		if (value->isConvertibleTo(Json::stringValue)) {
			return value->asString();
		}
		return default_value ? string(default_value) : string();
	}

	string JsonDocument::AsString(PJsonValue value, const string& default_value) {
		if (value->isConvertibleTo(Json::stringValue)) {
			return value->asString();
		}
		return default_value;
	}

	JsonDocument::JsonDocument()
		: root_(std::make_shared<Json::Value>(Json::objectValue)) {
	}

	bool JsonDocument::Parse(const string& doc) {
		Json::Reader reader;
		return reader.parse(doc.c_str(), *root_);
	}

	string JsonDocument::Print(bool pretty/* = true*/) const {
		return jsoninternal::Write(*root_, pretty);
	}

	string JsonDocument::Print(const string& key, bool pretty/* = true*/, bool dot_notation_enabled/* = true*/) const {
		Json::Value value;
		if (jsoninternal::Get(*root_, key, value, dot_notation_enabled)) {
			return jsoninternal::Write(value, pretty);
		}
		return string();
	}

	JsonDocument& JsonDocument::SetNull(const string& key, bool dot_notation_enabled/* = true*/) {
		jsoninternal::Set(*root_, key, Json::Value(Json::nullValue), dot_notation_enabled);
		return *this;
	}

	JsonDocument& JsonDocument::SetBool(const string& key, bool value, bool dot_notation_enabled/* = true*/) {
		jsoninternal::Set(*root_, key, Json::Value(value), dot_notation_enabled);
		return *this;
	}

	JsonDocument& JsonDocument::SetInt64(const string& key, int64_t value, bool dot_notation_enabled/* = true*/) {
		jsoninternal::Set(*root_, key, Json::Value(static_cast<Json::Value::Int64>(value)), dot_notation_enabled);
		return *this;
	}

	JsonDocument& JsonDocument::SetFloat(const string& key, float value, bool dot_notation_enabled/* = true*/) {
		jsoninternal::Set(*root_, key, Json::Value(value), dot_notation_enabled);
		return *this;
	}

	JsonDocument& JsonDocument::SetDouble(const string& key, double value, bool dot_notation_enabled/* = true*/) {
		jsoninternal::Set(*root_, key, Json::Value(value), dot_notation_enabled);
		return *this;
	}

	JsonDocument& JsonDocument::Set(const string& key, PJsonValue value, bool dot_notation_enabled/* = true*/) {
		jsoninternal::Set(*root_, key, *value, dot_notation_enabled);
		return *this;
	}

	JsonDocument& JsonDocument::Set(const string& key, const char* value, bool dot_notation_enabled/* = true*/) {
		jsoninternal::Set(*root_, key, Json::Value(value), dot_notation_enabled);
		return *this;
	}

	JsonDocument& JsonDocument::Set(const string& key, const string& value, bool dot_notation_enabled/* = true*/) {
		jsoninternal::Set(*root_, key, Json::Value(value), dot_notation_enabled);
		return *this;
	}

	JsonDocument& JsonDocument::Set(const string& key, shared_ptr<JsonDocument> value, bool dot_notation_enabled/* = true*/) {
		jsoninternal::Set(*root_, key, *value->root_, dot_notation_enabled);
		return *this;
	}

	JsonDocument& JsonDocument::AppendBool(const string& key, bool value, bool dot_notation_enabled/* = true*/) {
		jsoninternal::Append(*root_, key, Json::Value(value), dot_notation_enabled);
		return *this;
	}

	JsonDocument& JsonDocument::AppendInt64(const string& key, int64_t value, bool dot_notation_enabled/* = true*/) {
		jsoninternal::Append(*root_, key, Json::Value(static_cast<Json::Value::Int64>(value)), dot_notation_enabled);
		return *this;
	}

	JsonDocument& JsonDocument::AppendFloat(const string& key, float value, bool dot_notation_enabled/* = true*/) {
		jsoninternal::Append(*root_, key, Json::Value(value), dot_notation_enabled);
		return *this;
	}

	JsonDocument& JsonDocument::AppendDouble(const string& key, double value, bool dot_notation_enabled/* = true*/) {
		jsoninternal::Append(*root_, key, Json::Value(value), dot_notation_enabled);
		return *this;
	}

	JsonDocument& JsonDocument::Append(const string& key, PJsonValue value, bool dot_notation_enabled/* = true*/) {
		jsoninternal::Append(*root_, key, *value, dot_notation_enabled);
		return *this;
	}

	JsonDocument& JsonDocument::Append(const string& key, const char* value, bool dot_notation_enabled/* = true*/) {
		jsoninternal::Append(*root_, key, Json::Value(value), dot_notation_enabled);
		return *this;
	}

	JsonDocument& JsonDocument::Append(const string& key, const string& value, bool dot_notation_enabled/* = true*/) {
		jsoninternal::Append(*root_, key, Json::Value(value), dot_notation_enabled);
		return *this;
	}

	JsonDocument& JsonDocument::Append(const string& key, shared_ptr<JsonDocument> value, bool dot_notation_enabled/* = true*/) {
		jsoninternal::Append(*root_, key, *value->root_, dot_notation_enabled);
		return *this;
	}

	JsonDocument& JsonDocument::SetArray(const string& key, bool dot_notation_enabled/* = true*/) {
		jsoninternal::Set(*root_, key, Json::Value(Json::arrayValue), dot_notation_enabled);
		return *this;
	}

	bool JsonDocument::HasKey(const string& key, bool dot_notation_enabled/* = true*/) const {
		Json::Value _;
		return jsoninternal::Get(*root_, key, _, dot_notation_enabled);
	}

	bool JsonDocument::IsEmpty() const {
		return root_->empty();
	}

	bool JsonDocument::IsNull(const string& key, bool dot_notation_enabled/* = true*/) const {
		Json::Value _;
		if (!jsoninternal::Get(*root_, key, _, dot_notation_enabled)) {
			return false;
		}
		return _.isNull();
	}

	bool JsonDocument::IsNumeric(const string& key, bool dot_notation_enabled/* = true*/) const {
		Json::Value _;
		if (!jsoninternal::Get(*root_, key, _, dot_notation_enabled)) {
			return false;
		}
		return _.isNumeric();;
	}

	bool JsonDocument::IsString(const string& key, bool dot_notation_enabled/* = true*/) const {
		Json::Value _;
		if (!jsoninternal::Get(*root_, key, _, dot_notation_enabled)) {
			return false;
		}
		return _.isString();
	}

	bool JsonDocument::IsArray(const string& key, bool dot_notation_enabled/* = true*/) const {
		Json::Value _;
		if (!jsoninternal::Get(*root_, key, _, dot_notation_enabled)) {
			return false;
		}
		return _.isArray();
	}

	bool JsonDocument::IsDocument(const string& key, bool dot_notation_enabled/* = true*/) const {
		Json::Value _;
		if (!jsoninternal::Get(*root_, key, _, dot_notation_enabled)) {
			return false;
		}
		return _.isObject();
	}

	int JsonDocument::ArraySize(const string& key, bool dot_notation_enabled/* = true*/) const {
		Json::Value _;
		if (!jsoninternal::Get(*root_, key, _, dot_notation_enabled) || !_.isArray()) {
			return -1;
		}
		return _.size();
	}

	bool JsonDocument::AsBool(const string& key, bool default_value/* = false*/, bool dot_notation_enabled/* = true*/) const {
		Json::Value _;
		if (jsoninternal::Get(*root_, key, _, dot_notation_enabled)) {
			return AsBool(&_, default_value);
		}
		return default_value;
	}

	int64_t JsonDocument::AsInt64(const string& key, int64_t default_value/* = 0*/, bool dot_notation_enabled/* = true*/) const {
		Json::Value _;
		if (jsoninternal::Get(*root_, key, _, dot_notation_enabled)) {
			return AsInt64(&_, default_value);
		}
		return default_value;
	}

	int JsonDocument::AsInt(const string& key, int default_value/* = 0*/, bool dot_notation_enabled/* = true*/) const {
		return static_cast<int>(AsInt64(key, default_value, dot_notation_enabled));
	}

	float JsonDocument::AsFloat(const string& key, float default_value/* = 0.0f*/, bool dot_notation_enabled/* = true*/) const {
		Json::Value _;
		if (jsoninternal::Get(*root_, key, _, dot_notation_enabled)) {
			if (_.isConvertibleTo(Json::realValue)) {
				return _.asFloat();
			} else if (_.isString()) {
				return std::strtof(_.asCString(), nullptr);
			}
		}
		return default_value;
	}

	double JsonDocument::AsDouble(const string& key, double default_value/* = 0.0*/, bool dot_notation_enabled/* = true*/) const {
		Json::Value _;
		if (jsoninternal::Get(*root_, key, _, dot_notation_enabled)) {
			if (_.isConvertibleTo(Json::realValue)) {
				return _.asDouble();
			} else if (_.isString()) {
				return std::strtod(_.asCString(), nullptr);
			}
		}
		return default_value;
	}

	string JsonDocument::AsString(const string& key, bool dot_notation_enabled/* = true*/) const {
		Json::Value _;
		if (jsoninternal::Get(*root_, key, _, dot_notation_enabled) && _.isConvertibleTo(Json::stringValue)) {
			return _.asString();
		}
		return string();
	}

	string JsonDocument::AsString(const string& key, const char* default_value/* = nullptr*/, bool dot_notation_enabled/* = true*/) const {
		Json::Value _;
		if (jsoninternal::Get(*root_, key, _, dot_notation_enabled) && _.isConvertibleTo(Json::stringValue)) {
			return _.asString();
		}
		return default_value ? string(default_value) : string();
	}

	string JsonDocument::AsString(const string& key, const string& default_value, bool dot_notation_enabled/* = true*/) const {
		Json::Value _;
		if (jsoninternal::Get(*root_, key, _, dot_notation_enabled) && _.isConvertibleTo(Json::stringValue)) {
			return _.asString();
		}
		return default_value;
	}

	shared_ptr<JsonDocument> JsonDocument::AsDocument(const string& key, bool dot_notation_enabled/* = true*/) const {
		auto document = std::make_shared<JsonDocument>();
		jsoninternal::Get(*root_, key, *document->root_, dot_notation_enabled);
		return document;
	}

	vector<bool> JsonDocument::AsBoolArray(const string& key, bool dot_notation_enabled/* = true*/) const {
		vector<bool> values;
		Json::Value _;
		if (jsoninternal::Get(*root_, key, _, dot_notation_enabled) && _.isArray()) {
			for (auto _it : _) {
				if (_it.isConvertibleTo(Json::booleanValue)) {
					values.push_back(_it.asBool());
				}
			}
		}
		return values;
	}

	vector<int64_t> JsonDocument::AsInt64Array(const string& key, bool dot_notation_enabled/* = true*/) const {
		vector<int64_t> values;
		Json::Value _;
		if (jsoninternal::Get(*root_, key, _, dot_notation_enabled) && _.isArray()) {
			for (auto _it : _) {
				if (_it.isConvertibleTo(Json::booleanValue)) {
					values.push_back(_it.asBool());
				}
			}
		}
		return values;
	}

	vector<float> JsonDocument::AsFloatArray(const string& key, bool dot_notation_enabled/* = true*/) const {
		vector<float> values;
		Json::Value _;
		if (jsoninternal::Get(*root_, key, _, dot_notation_enabled) && _.isArray()) {
			for (auto _it : _) {
				if (_it.isConvertibleTo(Json::realValue)) {
					values.push_back(_it.asFloat());
				}
			}
		}
		return values;
	}

	vector<double> JsonDocument::AsDoubleArray(const string& key, bool dot_notation_enabled/* = true*/) const {
		vector<double> values;
		Json::Value _;
		if (jsoninternal::Get(*root_, key, _, dot_notation_enabled) && _.isArray()) {
			for (auto _it : _) {
				if (_it.isConvertibleTo(Json::realValue)) {
					values.push_back(_it.asDouble());
				}
			}
		}
		return values;
	}

	vector<string> JsonDocument::AsStringArray(const string& key, bool dot_notation_enabled/* = true*/) const {
		vector<string> values;
		Json::Value _;
		if (jsoninternal::Get(*root_, key, _, dot_notation_enabled) && _.isArray()) {
			for (auto _it : _) {
				if (_it.isConvertibleTo(Json::stringValue)) {
					values.push_back(_it.asString());
				}
			}
		}
		return values;
	}

	vector<shared_ptr<JsonDocument>> JsonDocument::AsDocumentArray(const string& key, bool dot_notation_enabled/* = true*/) const {
		vector<shared_ptr<JsonDocument>> values;
		Json::Value _;
		if (jsoninternal::Get(*root_, key, _, dot_notation_enabled) && _.isArray()) {
			for (auto _it : _) {
				if (_it.isObject()) {
					auto document = std::make_shared<JsonDocument>();
					*document->root_ = _it;
					values.push_back(document);
				}
			}
		}
		return values;
	}

	JsonDocument& JsonDocument::Merge(shared_ptr<JsonDocument> src, bool dot_notation_enabled/* = true*/) {
		Json::Value::Members members(src->root_->getMemberNames());
		for (auto it : members) {
			jsoninternal::Set(*root_, it, (*src->root_)[it], dot_notation_enabled);
		}
		return *this;
	}

	JsonDocument& JsonDocument::Copy(shared_ptr<JsonDocument> src, const string& key, const string& new_key, bool create_key_if_not_exists/* = false*/, bool dot_notation_enabled/* = true*/) {
		Json::Value _;
		if (jsoninternal::Get(*src->root_, key, _, dot_notation_enabled)) {
			jsoninternal::Set(*root_, new_key, _, dot_notation_enabled);
		} else if (create_key_if_not_exists) {
			jsoninternal::Set(*root_, new_key, Json::Value(Json::nullValue), dot_notation_enabled);
		}
		return *this;
	}

	JsonDocument& JsonDocument::Copy(shared_ptr<JsonDocument> src, const vector<string>& keys, const vector<string>& new_keys, bool create_key_if_not_exists/* = false*/, bool dot_notation_enabled/* = true*/) {
		for (auto key : keys) {
			Json::Value _;
			if (jsoninternal::Get(*src->root_, key, _, dot_notation_enabled)) {
				jsoninternal::Set(*root_, key, _, dot_notation_enabled);
			} else if (create_key_if_not_exists) {
				jsoninternal::Set(*root_, key, Json::Value(Json::nullValue), dot_notation_enabled);
			}
		}
		return *this;
	}

	JsonDocument& JsonDocument::Merge(shared_ptr<JsonDocument> src, const string& key, bool create_key_if_not_exists/* = false*/, bool dot_notation_enabled/* = true*/) {
		return Copy(src, key, key, create_key_if_not_exists, dot_notation_enabled);
	}

	JsonDocument& JsonDocument::Merge(shared_ptr<JsonDocument> src, const vector<string>& keys, bool create_key_if_not_exists/* = true*/, bool dot_notation_enabled/* = true*/) {
		return Copy(src, keys, keys, create_key_if_not_exists, dot_notation_enabled);
	}

	JsonDocument& JsonDocument::Move(const string& old_key, const string& new_key, bool dot_notation_enabled/* = true*/) {
		Json::Value _;
		if (jsoninternal::Remove(*root_, old_key, _, dot_notation_enabled)) {
			jsoninternal::Set(*root_, new_key, _, dot_notation_enabled);
		}
		return *this;
	}

	shared_ptr<JsonDocument> JsonDocument::Remove(const string& key, bool dot_notation_enabled/* = true*/) {
		auto removed_doc = std::make_shared<JsonDocument>();
		Json::Value _;
		if (jsoninternal::Remove(*root_, key, _, dot_notation_enabled)) {
			jsoninternal::Set(*removed_doc->root_, key, _, dot_notation_enabled);
		}
		return removed_doc;
	}

	string JsonDocument::RemoveAsString(const string& key, bool dot_notation_enabled/* = true*/) {
		Json::Value _;
		if (jsoninternal::Remove(*root_, key, _, dot_notation_enabled)) {
			return jsoninternal::Write(_, false);
		}
		return "";
	}

	JsonDocument& JsonDocument::RenameKey(const string& old_key, const string& new_key, int max_depth/* = -1*/) {
		jsoninternal::RenameKeys(*root_, [&](const string& key)->string {
			return (key == old_key) ? new_key : key;
		}, 1, max_depth);
		return *this;
	}

	JsonDocument& JsonDocument::RenameKeys(string(*rename_handler)(const string&), int max_depth/* = -1*/) {
		jsoninternal::RenameKeys(*root_, rename_handler, 1, max_depth);
		return *this;
	}

	shared_ptr<JsonDocument> JsonDocument::DupKey(const string& key, bool dot_notation_enabled/* = true*/) const {
		auto dup_doc = std::make_shared<JsonDocument>();
		Json::Value _;
		if (jsoninternal::Get(*root_, key, _, dot_notation_enabled)) {
			jsoninternal::Set(*dup_doc->root_, key, _, dot_notation_enabled);
		}
		return dup_doc;
	}

	shared_ptr<JsonDocument> JsonDocument::DupKeys(const vector<string>& keys, bool dot_notation_enabled/* = true*/) const {
		auto dup_doc = std::make_shared<JsonDocument>();
		for (auto key : keys) {
			Json::Value _;
			if (jsoninternal::Get(*root_, key, _, dot_notation_enabled)) {
				jsoninternal::Set(*dup_doc->root_, key, _, dot_notation_enabled);
			}
		}
		return dup_doc;
	}

	shared_ptr<JsonDocument> JsonDocument::DotNotation() const {
		auto dup_doc = std::make_shared<JsonDocument>();
		jsoninternal::EnumerateRootValue(root_.get(), [](Json::Value* parent, const string& path, const string& key, JsonValue* value, shared_ptr<void> doc)->bool {
			auto root = std::static_pointer_cast<Json::Value>(doc);	
			string pkey = key;
			if (!path.empty())
				pkey = path + "." + key;
			if (parent) {
				if (parent->isArray()) {
					jsoninternal::Append(*root, pkey, *value, false);
				} else {
					jsoninternal::Set(*root, pkey, *value, false);
				}
			}
			return true;
		}, dup_doc->root_);
		return dup_doc;
	}

	vector<string> JsonDocument::Keys() const {
		return root_->getMemberNames();
	}

	vector<string> JsonDocument::Values() const {
		vector<string> values;
		for (auto& it : root_->getMemberNames()) {
			values.push_back(jsoninternal::Write((*root_)[it], false));
		}
		return values;
	}
} // namespace simple

