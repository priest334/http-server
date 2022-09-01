#pragma once
#ifndef UTILS_SIMPLE_QUEUE_H_
#define UTILS_SIMPLE_QUEUE_H_

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>


using namespace std::chrono_literals;
using std::shared_ptr;
using std::deque;
using std::mutex;
using std::condition_variable;
namespace simple {
	template <class _DataType>	
	class SimpleQueue {
	public:
		typedef _DataType DataType;
		typedef deque<shared_ptr<DataType>> DataList;

		SimpleQueue()
			:enable_(true),
			mutex_(std::make_shared<mutex>()),
			notifier_(std::make_shared<condition_variable>()),
			data_list_(std::make_shared<DataList>()) {
		}

		void Push(shared_ptr<DataType> data) {
			if (!std::atomic_load(&enable_))
				return;
			data_list_->push_back(data);
			notifier_->notify_one();
		}

		shared_ptr<DataType> Get(bool nowait) {
			shared_ptr<DataType> data;
			std::unique_lock<std::mutex> lock(*mutex_);
			bool enable = std::atomic_load(&enable_);
			while (!nowait && enable && data_list_->empty()) {
				notifier_->wait_for(lock, 1000ms);
				enable = std::atomic_load(&enable_);
			}
			if (enable && !data_list_->empty()) {
				data = data_list_->front();
				data_list_->pop_front();
			}
			lock.unlock();
			return data;
		}

		shared_ptr<DataList> Get(int size, bool nowait) {
			auto data_list = std::make_shared<DataList>();
			std::unique_lock<std::mutex> lock(*mutex_);
			bool enable = std::atomic_load(&enable_);
			while (!nowait && enable && data_list_->empty()) {
				notifier_->wait_for(lock, 1000ms);
				enable = std::atomic_load(&enable_);
			}
			if (enable && !data_list_->empty()) {
				if (size <= 0) {
					data_list_.swap(data_list);
				} else {
					for (int i = 0; i < size && !data_list_->empty(); i++) {
						data_list->push_back(data_list_->front());
						data_list_->pop_front();
					}
				}
			}
			lock.unlock();
			return data_list;
		}

		void Enable(bool enable = true) {
			std::atomic_store(&enable_, enable);
		}
	private:
		std::atomic_bool enable_;
		shared_ptr<mutex> mutex_;
		shared_ptr<condition_variable> notifier_;
		shared_ptr<DataList> data_list_;
	};
} // namespace simple

#endif // UTILS_SIMPLE_QUEUE_H_

