#pragma once
#ifndef UTILS_XML_DOCUMENT_H_
#define UTILS_XML_DOCUMENT_H_

#include <memory>
#include <string>


using std::shared_ptr;
using std::string;

namespace tinyxml2 {
	class XMLDocument;
}

namespace simple {
	class XmlDocument {
	public:
		XmlDocument();
		XmlDocument(const string& data);

		bool Parse(const string& data);
		string Get(const string& field) const;
		string GetProperty(const string& field, const string& name) const;
		XmlDocument& SetEncoding(const string& encoding);
		XmlDocument& Set(const string& field, const string& value, bool cdata = true);
		XmlDocument& Set(const string& field, int value, bool cdata = false);
		XmlDocument& SetProperty(const string& field, const string& name, const string& value);
		XmlDocument& SetProperty(const string& field, const string& name, int value);
		string Print() const;
	private:
		shared_ptr<tinyxml2::XMLDocument> doc_;
	};
} // namespace simple

#endif // UTILS_XML_DOCUMENT_H_

