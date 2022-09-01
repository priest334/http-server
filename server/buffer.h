#pragma once
#ifndef SERVER_BUFFER_H_
#define SERVER_BUFFER_H_

#include <vector>


namespace simple {
	typedef std::vector<char> read_buffer_t;
	typedef std::vector<char> write_buffer_t;
} // namespace simple

#endif // SERVER_BUFFER_H_

