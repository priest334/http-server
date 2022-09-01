#pragma once
#ifndef UTILS_THREAD_LOCAL_H_
#define UTILS_THREAD_LOCAL_H_

#if defined(OS_WIN) && !defined(USE_POSIX_PTHREAD)
#	define TlsKey DWORD
#	define TlsCreate(key,destructor) key=TlsAlloc()
#	define TlsDestroy(key) TlsFree(key)
#	define TlsSet(key,value) TlsSetValue(key,value)
#	define TlsGet(key) TlsGetValue(key)
#else // USE_POSIX_PTHREAD
#include <pthread.h>
#	define TlsKey pthread_key_t
#	define TlsCreate(key,destructor) pthread_key_create(&key, destructor)
#	define TlsDestroy(key) pthread_key_delete(key)
#	define TlsSet(key,value) pthread_setspecific(key,value)
#	define TlsGet(key) pthread_getspecific(key)
#endif


namespace simple {	
	template <class _Type>
	class ThreadLocal {
		TlsKey index_;
		ThreadLocal(const ThreadLocal&) = delete;
		ThreadLocal& operator=(const ThreadLocal&) = delete;
	public:
		ThreadLocal() {
			TlsCreate(index_, nullptr);
		}

		~ThreadLocal() {
			TlsDestroy(index_);
		}

		_Type* Get() {
			return static_cast<_Type*>(TlsGet(index_));
		}

		void Set(void* value) {
			TlsSet(index_, value);
		}
	};

} // namespace simple

#endif // UTILS_THREAD_LOCAL_H_

