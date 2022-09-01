#include "task_runner.h"


namespace simple {
	void TaskRunner::ThreadProc(void* arg) {
		static_cast<TaskRunner*>(arg)->MainLoop();
	}

	void TaskRunner::MainLoop() {
		while (!closed_) {
			auto tasks = task_queue_->Get(cache_, closing_);
			for (auto task : *tasks) {
				task->Run();
				task->Done();
			}
			if (closing_ && tasks->empty()) {
				break;
			}
		}
	}

	TaskRunner::TaskRunner(int cache/* = 1*/)
		: closed_(true),
		closing_(false),
		cache_(cache),
		task_queue_(std::make_shared<TaskQueue>()) {
	}

	TaskRunner::~TaskRunner() {
		Close();
	}

	void TaskRunner::Start(int size) {
		closed_ = false;
		for (int i = 0; i < size; i++) {
			workers_.push_back(std::make_shared<std::thread>(&TaskRunner::ThreadProc, this));
		}
	}

	void TaskRunner::Join() {
		for (auto it = workers_.begin(); it != workers_.end(); ++it) {
			(*it)->join();
		}
		workers_.clear();
	}

	void TaskRunner::Push(shared_ptr<Task> task) {
		if (closing_ || closed_)
			return;
		task_queue_->Push(task);
	}

	void TaskRunner::Close(bool gracefully/* = false*/) {
		if (gracefully) {
			closing_ = true;
		} else {
			closed_ = true;
		}
		task_queue_->Enable(false);
		Join();
	}
} // namespace simple

