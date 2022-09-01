#pragma once
#ifndef UTILS_TASK_RUNNER_H_
#define UTILS_TASK_RUNNER_H_

#include <atomic>
#include <thread>
#include <vector>
#include "task.h"
#include "simple_queue.h"


using std::thread;
using std::vector;
namespace simple {
	typedef SimpleQueue<Task> TaskQueue;
	class TaskRunner {
		static void ThreadProc(void* arg);
		void MainLoop();
	public:
		TaskRunner(int cache = 1);
		~TaskRunner();
		void Start(int size);
		void Join();
		void Push(shared_ptr<Task> task);
		void Close(bool gracefully = false);
	private:
		std::atomic_bool closed_;
		std::atomic_bool closing_;
		int cache_;
		vector<shared_ptr<thread>> workers_;
		shared_ptr<TaskQueue> task_queue_;
	};
} // namespace simple

#endif // UTILS_TASK_RUNNER_H_

