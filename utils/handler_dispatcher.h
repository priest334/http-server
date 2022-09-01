#ifndef UTILS_HANDLER_DISPATCHER_H_
#define UTILS_HANDLER_DISPATCHER_H_
#pragma once

#include <memory>
#include <string>
#include <unordered_map>


using std::shared_ptr;
using std::string;
using std::unordered_map;

template <class HandlerType>
class HandlerDispatcher {
	HandlerDispatcher(const HandlerDispatcher&) = delete;
	HandlerDispatcher& operator=(const HandlerDispatcher&) = delete;
public:
	void Install(shared_ptr<HandlerType> handler) {
		handlers_[handler->HandlerName()] = std::static_pointer_cast<HandlerType>(handler);
	}

	shared_ptr<HandlerType> FindHandler(const string& name) const {
		auto it = handlers_.find(name);
		if (it != handlers_.end())
			return it->second;
		return nullptr;
	}

private:
	unordered_map<string, shared_ptr<HandlerType>> handlers_;
};

#endif // UTILS_HANDLER_DISPATCHER_H_

