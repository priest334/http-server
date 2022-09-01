#pragma once
#ifndef UTILS_TASK_H_
#define UTILS_TASK_H_

#include <string>


using std::string;
namespace simple {
	class Task {
	public:
		Task();
		Task(const string& id);
		string Id() const;
		virtual ~Task();
		virtual void Run() = 0;
		virtual void Done() = 0;
	protected:
		string id_;
	};
} // namespace simple

#endif // UTILS_TASK_H_

