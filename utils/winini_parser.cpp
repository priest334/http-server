#include "winini_parser.h"

#include <iostream>
#include <sstream>
#include "config_item.h"
#include "config.h"


using std::ifstream;
using std::ofstream;
namespace simple {
	namespace {
		enum class state {
			kStart,
			kKey,
			kAssign,
			kValue,
			kSectionStart,
			kSection,
			kSectionEnd,
			kComment,
			kBlank,
			kNewLine,
			kEnd,
			kError
		};

		state next(state current, char ch) {
			switch (ch) {
			case ' ':
			case '\t':
			case '\r':
				return state::kBlank;
			case '\n': {
				switch (current) {
				case state::kKey:
				case state::kSection:
				case state::kSectionStart:
					return state::kError;
				default:
					return state::kNewLine;
				}
			}
			}
			switch (current) {
			case state::kStart: {
				switch (ch) {
				case ';':
					return state::kComment;
				case '[':
					return state::kSectionStart;
				default:
					return state::kError;
				}
			}
			case state::kNewLine: {
				switch (ch) {
				case ';':
					return state::kComment;
				case '[':
					return state::kSectionStart;
				default:
					return state::kKey;
				}
			}
			case state::kKey: {
				switch (ch) {
				case '=':
					return state::kAssign;
				default:
					return state::kKey;
				}
			}
			case state::kAssign: {
				return state::kValue;
			}
			case state::kValue: {
				return state::kValue;
			}
			case state::kComment: {
				return state::kComment;
			}
			case state::kSectionStart: {
				switch (ch) {
				case ']':
					return state::kSectionEnd;
				default:
					return state::kSection;
				}
			}
			case state::kSection: {
				switch (ch) {
				case ']':
					return state::kSectionEnd;
				default:
					return state::kSection;
				}
			}
			case state::kSectionEnd: {
				return state::kError;
			}
			default:
				return state::kEnd;
			}
		}
	}

	WinIniParser::WinIniParser(Config* config)
		: config_(config) {
	}

	WinIniParser::~WinIniParser() {
	}

	bool WinIniParser::Load(const string& text) {		
		int i, ks, ke, vs, ve, ss, ne, len;
		ks = ke = vs = ve = ss = ne = -1;
		len = (int)text.length();
		state current = state::kStart;
		shared_ptr<ConfigItem> section;
		for (i = 0; i < len; i++) {
			char ch = text[i];		
			state old = current;
			state nxt = next(current, ch);
			if (nxt == state::kError)
				return false;
			if (nxt != state::kBlank)
				current = nxt;
			else
				continue;
			if (current == state::kKey) {
				if (old != state::kKey) {
					ks = i;
					ke = ks;
				} else {
					ke = i;
				}
			} else if (current == state::kValue) {
				if (old != state::kValue) {
					vs = i;
					ve = vs;
				} else {
					ve = i;
				}
			} else if (current == state::kSection) {
				if (old != state::kSection) {
					ss = i;
					ne = ss;
				} else {
					ne = i;
				}
			}		
			if (current == state::kNewLine) {
				if (ss >= 0 && ne >= ss) {
					string name = text.substr(ss, ne - ss + 1);
					section.reset(new ConfigItem(name));
					config_->Root()->Append(name, section);
				} else {
					string name, value;
					if (ks >=0 && ke >= ks) {
						name = text.substr(ks, ke - ks + 1);
					}
					if (vs >= 0 && ve >= vs) {
						value = text.substr(vs, ve - vs + 1);
					}
					if (ks > 0 || vs > 0) {
						section->SetProperty(name, value);
					}
				}
				ks = ke = vs = ve = ss = ne = -1;
			}
		}
		return true;
	}

	string WinIniParser::Save(ConfigItem* root) const {
		std::ostringstream oss;
		auto children = root->Children();
		auto it = children.begin();
		for (; it != children.end(); ++it) {
			oss << "[" << it->first << "]" << std::endl;
			auto properties = it->second->Properties();
			auto itp = properties.begin();
			for (; itp != properties.end(); ++itp) {
				oss << itp->first << " = " << itp->second << std::endl;
			}
		}
		return oss.str();
	}
} // namespace simple

