#ifndef THREADING_H_
#define THREADING_H_

#ifdef __CPP11

#include <thread>
#include <condition_variable>
#include <atomic>

#include "Util.h"

namespace perf
{
	class ThreadPool
	{
	public:
		ThreadPool();
		virtual ~ThreadPool();

		void doWork(void (*func)(uchar, uchar, void*, void*, void*, void*), void * = NULL, void * = NULL, void * = NULL, void * = NULL);

	private:
		bool isWorkAvailable() const { return mWorkAvailable; }
		void wait(uchar);

		const uchar mThreadNum;
		std::vector<std::thread> mThreads;
		std::condition_variable mCondition;
		std::mutex mMutex;
		std::mutex mWorkMutex;
		std::atomic<uchar> mAtomicCount;
		bool mThreadExit;
		bool mWorkAvailable;
		void (*mFunc)(uchar, uchar, void*, void*, void*, void*);
		void * mArg1;
		void * mArg2;
		void * mArg3;
		void * mArg4;
	};
}

#endif

#endif