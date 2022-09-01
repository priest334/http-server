#include "xml_document.h"

#include <sstream>
#include "third_party/tinyxml2/tinyxml2.h"


namespace simple {
	XmlDocument::XmlDocument()
		: doc_(new tinyxml2::XMLDocument()) {
		doc_->InsertEndChild(doc_->NewElement("xml"));
	}

	XmlDocument::XmlDocument(const string& data)
		: doc_(new tinyxml2::XMLDocument()) {
		Parse(data);
	}

	bool XmlDocument::Parse(const string& data) {
		return (tinyxml2::XML_SUCCESS == doc_->Parse(data.c_str(), data.size()));
	}

	string XmlDocument::Get(const string& field) const {
		string value;
		tinyxml2::XMLElement* xml = doc_->FirstChildElement("xml");
		if (xml) {
			tinyxml2::XMLElement* child = xml->FirstChildElement(field.c_str());
			if (child) {
				const char* text = child->GetText();
				if (text) {
					value.assign(text);
				}
			}
		}
		return value;
	}

	string XmlDocument::GetProperty(const string& field, const string& name) const {
		string value;
		tinyxml2::XMLElement* xml = doc_->FirstChildElement("xml");
		if (xml) {
			tinyxml2::XMLElement* child = xml->FirstChildElement(field.c_str());
			if (child) {
				const char* text = child->Attribute(name.c_str());
				if (text) {
					value.assign(text);
				}
			}
		}
		return value;
	}

	XmlDocument& XmlDocument::SetEncoding(const string& encoding) {
		tinyxml2::XMLElement* xml = doc_->FirstChildElement("xml");
		if (xml) {
			xml->SetAttribute("encoding", encoding.c_str());
		}
		return *this;
	}

	XmlDocument& XmlDocument::Set(const string& field, const string& value, bool cdata/* = true*/) {
		tinyxml2::XMLElement* xml = doc_->FirstChildElement("xml");
		if (xml) {
			tinyxml2::XMLElement* child = xml->FirstChildElement(field.c_str());
			tinyxml2::XMLText* text = doc_->NewText(value.c_str());
			text->SetCData(cdata);
			if (child) {
				xml->DeleteChild(child);
			}
			child = doc_->NewElement(field.c_str());
			child->InsertEndChild(text);
			xml->InsertEndChild(child);
		}
		return *this;
	}

	XmlDocument& XmlDocument::Set(const string& field, int value, bool cdata/* = false*/) {
		std::ostringstream oss;
		oss << value;
		return Set(field, oss.str(), cdata);
	}

	XmlDocument& XmlDocument::SetProperty(const string& field, const string& name, const string& value) {
		tinyxml2::XMLElement* xml = doc_->FirstChildElement("xml");
		if (xml) {
			tinyxml2::XMLElement* child = xml->FirstChildElement(field.c_str());
			if (child) {
				child->SetAttribute(name.c_str(), value.c_str());
			} else {
				child = doc_->NewElement(field.c_str());
				child->SetAttribute(name.c_str(), value.c_str());
				xml->InsertEndChild(child);
			}
		}
		return *this;
	}

	XmlDocument& XmlDocument::SetProperty(const string& field, const string& name, int value) {
		tinyxml2::XMLElement* xml = doc_->FirstChildElement("xml");
		if (xml) {
			tinyxml2::XMLElement* child = xml->FirstChildElement(field.c_str());
			if (child) {
				child->SetAttribute(name.c_str(), value);
			} else {
				child = doc_->NewElement(field.c_str());
				child->SetAttribute(name.c_str(), value);
				xml->InsertEndChild(child);
			}
		}
		return *this;
	}

	string XmlDocument::Print() const {
		tinyxml2::XMLPrinter printer;
		doc_->Print(&printer);
		return string(printer.CStr());
	}
} // namespace simple

