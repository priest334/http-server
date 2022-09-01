#include "task.h"

#include "uuid_helper.h"


namespace simple {
	Task::Task() 
		: id_(Uuid().ToString().c_str()) {
	}

	Task::Task(const string& id)
		: id_(id.c_str()) {
	}

	string Task::Id() const {
		return id_;
	}

	Task::~Task() {
	}
} // namespace simple

