#include "uuid_helper.h"


namespace simple {
	Uuid::Uuid() {
#if defined (_WIN32)
		HRESULT hr = CoCreateGuid(&uuid_);
#elif defined(__linux__)
		uuid_generate(uuid_);
#endif
	}

	string Uuid::ToString(bool upper/* = false*/) const {
		char buffer[64] = { 0 };
		const char* const lower_fmt = "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x";
		const char* const upper_fmt = "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X";
#if defined (_WIN32)
		snprintf(buffer, 64, (upper?upper_fmt:lower_fmt),
			uuid_.Data1, uuid_.Data2, uuid_.Data3,
			uuid_.Data4[0], uuid_.Data4[1],
			uuid_.Data4[2], uuid_.Data4[3], uuid_.Data4[4],
			uuid_.Data4[5], uuid_.Data4[6], uuid_.Data4[7]);
#elif defined(__linux__)
		snprintf(buffer, 64, (upper ? upper_fmt : lower_fmt),
			*((unsigned int*)uuid_), *((unsigned short*)(uuid_ +4)), *((unsigned short*)(uuid_ + 6)),
			uuid_[8], uuid_[9],
			uuid_[10], uuid_[11], uuid_[12],
			uuid_[13], uuid_[14], uuid_[15]);
#endif
		return string(buffer);
	}
} // namespace simple

