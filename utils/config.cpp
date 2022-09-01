#include "config.h"

#include <fstream>
#include <sstream>
#include "config_item.h"
#include "winini_parser.h"


namespace simple {
	string Config::Get(shared_ptr<ConfigItem> parent, const string& name) const {
		if (name.empty())
			return name;
		if (parent->HasProperty(name))
			return parent->GetProperty(name);
		string::size_type fpos = name.find(".");
		if (fpos != string::npos) {
			string child_name = name.substr(0, fpos);
			auto child = parent->GetChild(child_name);
			if (child)
				return Get(child, name.substr(fpos + 1));
		}
		return string();
	}

	bool Config::Set(shared_ptr<ConfigItem> parent, const string& name, const string& value) {
		if (name.empty())
			return false;
		string::size_type fpos = name.find(".");
		if (fpos != string::npos) {				
			string child_name = name.substr(0, fpos);
			auto child = parent->GetChild(child_name);
			if (!child) {
				child = std::make_shared<ConfigItem>(child_name);
				parent->Append(child_name, child);
			}
			return Set(child, name.substr(fpos + 1), value);
		} else {
			parent->SetProperty(name, value);
			return true;
		}
	}

	Config::Config()
		: parser_(new WinIniParser(this)),
		root_(new ConfigItem(string())) {
	}

	bool Config::Load(const string& filename) {
		std::ifstream in(filename, std::ios_base::in | std::ios_base::binary);
		if (in.is_open()) {
			in.seekg(0, in.end);
			size_t len = (size_t)in.tellg();
			in.seekg(0, in.beg);
			char* data = new char[len];
			in.read(data, len);
			bool retval = parser_->Load(string(data, len));
			delete[] data;
			return retval;
		}
		return false;
	}

	bool Config::Save(const string& filename) {
		std::ofstream out(filename, std::ios_base::out | std::ios_base::binary);
		if (out.is_open()) {
			string text = parser_->Save(root_.get());
			out.write(text.c_str(), text.length());
			return true;
		}
		return false;
	}

	shared_ptr<ConfigItem> Config::Root() const {
		return root_;
	}

	string Config::Get(const string& name) const {
		return Get(root_, name);
	}

	string Config::Get(const string& name, const string& default_value) {
		string value = Get(root_, name);
		if (value.empty()) {
			return default_value;
		}
		return value;
	}

	int64_t Config::GetInt(const string& name) const {
		string value = Get(root_, name);
		if (value.empty())
			return 0;
		return atoi(value.c_str());
	}

	int64_t Config::GetInt(const string& name, int64_t default_value) const {
		string value = Get(root_, name);
		if (value.empty())
			return default_value;
		return atoi(value.c_str());
	}

	bool Config::Set(const string& name, const string& value) {
		return Set(root_, name, value);
	}

	bool Config::SetInt(const string& name, int64_t value) {
		std::ostringstream oss;
		oss << value;
		return Set(root_, name, oss.str());
	}

	unordered_map<string, string> Config::GetAll(const string& name) const {
		auto child = root_->GetChild(name);
		if (child) {
			return child->Properties();
		}
		return unordered_map<string, string>();
	}
} // namespace simple

