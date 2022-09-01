#pragma once
#ifndef UTILS_UUID_HELPER_H_
#define UTILS_UUID_HELPER_H_

#include <string>
#if defined (_WIN32)
#include <objbase.h>
#define uuid_type_t GUID
#elif defined (__linux__)
#include <uuid/uuid.h>
#define uuid_type_t uuid_t
#else
#error "uuid_token not implemented"
#endif


using std::string;
namespace simple {
	class Uuid {
	public:
		Uuid();
		string ToString(bool upper = false) const;
	private:
		uuid_type_t uuid_;
	};
	
} // namespace simple

#endif // UTILS_UUID_HELPER_H_

